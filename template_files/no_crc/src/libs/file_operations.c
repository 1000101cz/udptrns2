#include "file_operations.h"
#include <time.h>

// send file to server from client
void send_file(char *file_dest, long n_o_char, int socket_descriptor, struct sockaddr_in server_address, int len) {
    long pointer = 0;
    int number_of_packets=1;
    unsigned char buffer[BUFFER_SIZE] = {'\0'};
    FILE *read_file = fopen(file_dest,"rb");
    unsigned long CRC_value = 0;
    _Bool sending_file = 1;
    int repeat_error = 0;
    long total_error_count = 0;


        while(1) {

            // clear buffer
            for (int i  = 0; i < BUFFER_SIZE; i++) { buffer[i] = '\0'; }

            // fill buffer
            for (int i  = 0; i < BUFFER_SIZE && pointer != n_o_char; i++) {
                buffer[i] = getc(read_file);
                pointer++;
            }


            // send buffer with data
            sendto(socket_descriptor, buffer, sizeof(unsigned char)*(BUFFER_SIZE),MSG_CONFIRM, (const struct sockaddr *) &server_address,sizeof(server_address));
            number_of_packets++;
            struct timespec delta = {0 /*secs*/, 1 /*nanosecs*/}; // this takes some time
            //while (nanosleep(&delta, &delta));



            if (pointer > n_o_char) {
                break; // exit main while loop
            }
            if (pointer == n_o_char) {
              break;
            }
        }

    printf("Data packets sent: %d \n",number_of_packets);
    printf("Total number of corrupted packets: %ld\n", total_error_count);

    fclose(read_file);
}

// receive file from client
void receive_message(char *file_dest, int socket_descriptor, struct sockaddr_in client_address, int len, long message_length) {
    unsigned char buffer[BUFFER_SIZE] = {'\0'};
    long counter = 0;
    long packet_counter = 0;
    long total_error_count = 0;
    FILE *new_file;
    new_file = fopen(file_dest,"wb");
    _Bool receiving_file = 1;
    int packet_error_count = 0;

    while (1) {
        recvfrom(socket_descriptor, buffer, sizeof(unsigned char)*(BUFFER_SIZE),MSG_WAITALL, ( struct sockaddr *) &client_address,(unsigned int*)&len);
        packet_counter++;

        for (int i = 0; i < BUFFER_SIZE; i++) {

            if (counter == message_length) { break; } // reached end of message
            fputc(buffer[i],new_file);
            counter++;
        }

        //printf("counter: %10ld  vs   %10ld\n",counter,message_length);

        if (counter >= message_length) { break; }
    }

    printf("Data packets received: %ld\n",packet_counter+1);
    fclose(new_file);
}
