// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../sha256/sha256.h"
#include "../libs/confirmations.h"
#include "../libs/file_operations.h"



int main(int argc, char *argv[]) {

    // ERROR messages
    if (argc < 3) {
        fprintf(stderr, "Enter arguments! (address and file destination)\n");
        return 100;
    }
    if ( access(argv[2], F_OK) != 0 ) {
        fprintf(stderr, "ERROR: File %s dont exist!\n", argv[2]);
        return 100;
    }

    // Create socket file descriptor
    int socket_descriptor;
    if ( (socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        fprintf(stderr, "ERROR: Socket file descriptor failed\n");
        return 100;
    }

    printf("File: %s\n",argv[2]);
    printf("Dest: %s:%d\n",argv[1],PORT);
    printf("----------------------\n");

    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));
    int len = sizeof(server_address);

    struct timeval stop, start;

    // fill server information
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(argv[1]);

    // compute length of file
    FILE *read_file;
    read_file = fopen(argv[2],"rb");
    fseek(read_file, 0, SEEK_END);
    long n_o_char = ftell(read_file);
    fclose(read_file);
    unsigned char str[BUFFER_SIZE] = {'\0'};
    sprintf((char*)str, "%ld", n_o_char);


    // send server length of file
    sendto(socket_descriptor, str, sizeof(unsigned char)*BUFFER_SIZE,MSG_CONFIRM, (const struct sockaddr *) &server_address,sizeof(server_address));
    printf("Connection estahblished...\n\nLength of file: %ld\n",n_o_char);


    // send file
    gettimeofday(&start, NULL);
    send_file(argv[2], n_o_char, socket_descriptor, server_address, len);
    gettimeofday(&stop, NULL);
    long time_taken = (stop.tv_sec - start.tv_sec)*1000 + (stop.tv_usec - start.tv_usec)/1000;
    double time_taken_s = time_taken/1000.0;
    printf("Time taken: %.3f s\n",time_taken_s);
    printf("Transfer speed: %.1f MB/s\n\n",(double)(n_o_char/1000000)/time_taken_s);


    // send SHA256 hash of file to server
    char hash[65] = {0}; // create empty array
    hash_file(argv[2],hash); // fill array with file hash
    for (int i = 0; i < BUFFER_SIZE; i++) { str[i] = '\0'; } // clear str buffer
    sprintf((char*)str, "%s", hash); // fill str with file hash
    sendto(socket_descriptor, str, sizeof(unsigned char)*BUFFER_SIZE,MSG_CONFIRM, (const struct sockaddr *) &server_address,sizeof(server_address)); // send file hash to server


    // wait for SHA256 hash confirmation
    if (get_last_confirmation(socket_descriptor, server_address, len)) {
      printf("\nSuccess\n");
      close(socket_descriptor);
    } else {
      fprintf(stderr,"Transfer failed - SHA256 hashes do not match!\n");
      close(socket_descriptor);
      exit(100);
    }

    return 0;
}
