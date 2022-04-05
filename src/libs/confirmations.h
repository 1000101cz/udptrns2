#ifndef CONFIRMATIONS_H__
#define CONFIRMATIONS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT     32222
#define BUFFER_SIZE 1024
#define SUB_BUFFER_SIZE 40
#define TIMEOUT_S 1
#define TIMEOUT_MS 0
#define CRC_SIZE 4
#define MAX_SENT_REPEAT 20

//#define GENERATE_ERRORS
#ifdef GENERATE_ERRORS
  #define DONT_SENT_CONF 900 // 1 in value (program will sleep for TIMEOUT each time)
  #define CORRUPT_PACKET 50 // 1 in value
#endif


// send Error message to client (buffer filled with 0)
void send_error_message(int socket_descriptor, struct sockaddr_in client_address);

// send Success message to client (buffer filled with 1)
void send_success_message(int socket_descriptor, struct sockaddr_in client_address);

// recieve confirmation that operation succeded
_Bool get_confirmation(int socket_descriptor, struct sockaddr_in server_address, int len);

// recieve last confirmation that operation succeded - no timeout
_Bool get_last_confirmation(int socket_descriptor, struct sockaddr_in server_address, int len);

#endif
