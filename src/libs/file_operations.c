#include "file_operations.h"

// send file to server from client
void send_file(char *file_dest, long n_o_char, int socket_descriptor, struct sockaddr_in server_address, int len) {
    long pointer = 0;
    int number_of_packets=1;
    unsigned char buffer[BUFFER_SIZE - SUB_BUFFER_SIZE] = {'\0'};
    unsigned char sub_buffer[SUB_BUFFER_SIZE] = {'\0'};
    unsigned char big_buffer[BUFFER_SIZE] = {'\0'};
    FILE *read_file = fopen(file_dest,"rb");
    unsigned long CRC_value = 0;
    _Bool sending_file = 1;
    int repeat_error = 0;
    long total_error_count = 0;

    while(sending_file) {
        // repeat if packet transfer failed
        repeat_error = 0;
        while(1) {
            if (repeat_error == 0) {
              // clear buffer
              for (int i  = 0; i < BUFFER_SIZE-SUB_BUFFER_SIZE; i++) { buffer[i] = '\0'; }

              // fill buffer
              for (int i  = 0; i < BUFFER_SIZE-SUB_BUFFER_SIZE && pointer != n_o_char; i++) {
                  buffer[i] = getc(read_file);
                  pointer++;
              }
            }

            // create CRC and pucket number subbuffer
            CRC_value = compute_CRC_buffer(&buffer,BUFFER_SIZE-SUB_BUFFER_SIZE);// compute CRC
            sprintf((unsigned char*)sub_buffer, "%ld %ld",number_of_packets, CRC_value);

            // connect buffers (data + CRC)
            for (int i = 0; i < BUFFER_SIZE-SUB_BUFFER_SIZE; i++) {
              big_buffer[i] = buffer[i];
            }
            for (int i = BUFFER_SIZE-SUB_BUFFER_SIZE; i < BUFFER_SIZE; i++) {
              big_buffer[i] = sub_buffer[i-(BUFFER_SIZE-SUB_BUFFER_SIZE)];
            }

            // send buffer with data, CRC and packet number
            sendto(socket_descriptor, big_buffer, sizeof(unsigned char)*(BUFFER_SIZE),MSG_CONFIRM, (const struct sockaddr *) &server_address,sizeof(server_address));
            number_of_packets++;

            // wait for confirmation
            if (!get_confirmation(socket_descriptor, server_address, len)) {
              number_of_packets--;
              repeat_error++;
              total_error_count++;
            } else{
              if (pointer >= n_o_char) {
                sending_file = 0; // exit main while loop
              }
              break;
            }
            if (repeat_error >= MAX_SENT_REPEAT) {
              fprintf(stderr,"Communication failed! Terminating..\n");
              fprintf(stderr,"Packets sent successfuly: %d\n",number_of_packets);
              fprintf(stderr,"Number of errors: %ld\n",total_error_count);
              exit(100);
            }
        }
    }

    printf("Data packets sent: %d \n",number_of_packets);
    printf("Total number of corrupted packets: %ld\n", total_error_count);

    fclose(read_file);
}

// receive file from client
void receive_message(char *file_dest, int socket_descriptor, struct sockaddr_in client_address, int len, long message_length) {
    unsigned char buffer[BUFFER_SIZE-SUB_BUFFER_SIZE] = {'\0'};
    unsigned char packet_number_crc[BUFFER_SIZE] = {'\0'};
    unsigned char packet_number_buffer[SUB_BUFFER_SIZE/2];
    unsigned char crc_buffer[SUB_BUFFER_SIZE/2];
    long counter = 0;
    long packet_counter = 0;
    long crc_received, packet_number, crc_computed;
    long total_error_count = 0;
    FILE *new_file;
    new_file = fopen(file_dest,"wb");
    _Bool receiving_file = 1;

    while (receiving_file) {
        while (1) {
            // receive packet with data number and CRC
            recvfrom(socket_descriptor, packet_number_crc, sizeof(unsigned char)*(BUFFER_SIZE),MSG_WAITALL, ( struct sockaddr *) &client_address,(unsigned int*)&len);
            packet_counter++;

            // separate number
            for (int i = BUFFER_SIZE-SUB_BUFFER_SIZE; i < BUFFER_SIZE; i++) {
              if (packet_number_crc[i] != ' ') {
                  packet_number_buffer[i-(BUFFER_SIZE-SUB_BUFFER_SIZE)] = packet_number_crc[i];
              } else { break; }
            }
            packet_number = atol(packet_number_buffer);

            // separate CRC
            for (int i = BUFFER_SIZE - SUB_BUFFER_SIZE; i < BUFFER_SIZE; i++) {
              if (packet_number_crc[i] == ' ') {
                for (int j = i; j < i+SUB_BUFFER_SIZE/2; j++) {
                  crc_buffer[j-1-i] = packet_number_crc[j];
                }
                break;
              }
            }
            crc_received = atol(crc_buffer);


            // clear buffers
            for (int i = 0; i < 20; i++) {
              packet_number_buffer[i] = '\0';
              crc_buffer[i] = '\0';
            }

            // separate data
            for (int i = 0; i < BUFFER_SIZE-SUB_BUFFER_SIZE; i++) {
              buffer[i] = packet_number_crc[i];
            }

            // compute CRC
            crc_computed =  compute_CRC_buffer(&buffer,BUFFER_SIZE-SUB_BUFFER_SIZE);

            // check CRC and file number
            if (crc_computed == crc_received && packet_number == packet_counter) {
              send_success_message(socket_descriptor, client_address);
              for (int i = 0; i < BUFFER_SIZE-SUB_BUFFER_SIZE; i++) {
                  if (counter >= message_length) {
                    receiving_file = 0;
                    break;
                  } // reached end of message
                  fputc(buffer[i],new_file);
                  counter++;
              }
              if (counter >= message_length) {  // reached end of message
                receiving_file = 0;
                break;
              }
            }
            else {
              total_error_count++;
              send_error_message(socket_descriptor, client_address);
              packet_counter--;
            }
        }
    }
    printf("Data packets received: %ld\n",packet_counter+1);
    printf("Total number of corrupted packets: %ld\n", total_error_count);
    fclose(new_file);
}
