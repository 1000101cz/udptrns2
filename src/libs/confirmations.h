#ifndef CONFIRMATIONS_H__
#define CONFIRMATIONS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT     8080
#define BUFFER_SIZE 1024
#define TIMEOUT_MS 10000
#define CRC_SIZE 4
#define MAX_SENT_REPEAT 100


// send Error message to client (buffer filled with 0)
void send_error_message(int socket_descriptor, struct sockaddr_in client_address);

// send Success message to client (buffer filled with 1)
void send_success_message(int socket_descriptor, struct sockaddr_in client_address);

// recieve confirmation that operation succeded
_Bool get_confirmation(int socket_descriptor, struct sockaddr_in server_address, int len);

// recieve last confirmation that operation succeded - no timeout
_Bool get_last_confirmation(int socket_descriptor, struct sockaddr_in server_address, int len);

#endif
