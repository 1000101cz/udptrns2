#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../sha256/sha256.h"
#include "../libs/confirmations.h"
#include "../libs/file_operations.h"


int main(int argc, char *argv[]) {

    // ERROR messages
    if (argc < 2) {
        fprintf(stderr,"ERROR: destination for new file required!\n");
        return 100;
    }
    char *file_dest = argv[1];
    if ( access(file_dest, F_OK) == 0 ) {
        fprintf(stderr,"ERROR: File exists!\n");
        return 100;
    }
    printf("File: %s\n",argv[1]);

    // Create socket file descriptor
    int socket_descriptor;
    if ( (socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { return 100; }

    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));
    int len = sizeof(client_address);

    // fill server information
    server_address.sin_family    = AF_INET; // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind socket with server address
    if ( bind(socket_descriptor, (const struct sockaddr *)&server_address,sizeof(server_address)) < 0 ) { return 100; }


    // get total length of message from the first packet
    unsigned char buffer[BUFFER_SIZE] = {'\0'};
    recvfrom(socket_descriptor, buffer, sizeof(unsigned char)*BUFFER_SIZE,MSG_WAITALL, ( struct sockaddr *) &client_address,(unsigned int*)&len);

    // get IP addrress of client (not necessary)
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_address;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, ip_str, INET_ADDRSTRLEN );
    printf("Source: %s\n",ip_str);
    printf("----------------------\n");
    printf("Connection established...\n\n");
    long message_length = atol(buffer);
    printf("Length of file: %ld\n",message_length);

    // receive message
    receive_message(file_dest, socket_descriptor, client_address, len, message_length);

    // compute SHA256 hash
    char hash[65] = {0}; // create empty hash array
    hash_file(file_dest,hash); // fill array with hash

    // receive SHA256 hash
    for (int i = 0; i < BUFFER_SIZE; i++) { buffer[i] = '\0'; } // clear buffer
    recvfrom(socket_descriptor, buffer, sizeof(unsigned char)*BUFFER_SIZE,MSG_WAITALL, ( struct sockaddr *) &client_address,(unsigned int*)&len); // receive hash

    // compare received and computed hashes
    _Bool fucked = 0;
    for (int i = 0; i < 65; i++) {
      if (hash[i] != buffer[i]) {
          fprintf(stderr, "ERROR: Hashes did not match!\n"); // send Error if they differ
          fucked = 1;
          send_error_message(socket_descriptor, client_address);
          break;
      }
    }
    if (!fucked) {
      printf("\nSuccess\n");
      send_success_message(socket_descriptor, client_address); // send Success if they match
    } else {
      fprintf(stderr,"ERROR: Transfer failed!\n");
    }

    return 0;
}
