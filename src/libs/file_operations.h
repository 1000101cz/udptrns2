#ifndef FILE_OPERATIONS_H__
#define FILE_OPERATIONS_H__

#include "confirmations.h"
#include "../crc32/crc32.h"


// send file from client to server
void send_file(char *file_dest, long n_o_char, int socket_descriptor, struct sockaddr_in server_address, int len);

// receive file from client
void receive_message(char *file_dest, int socket_descriptor, struct sockaddr_in client_address, int len, long message_length);

#endif
