#include "confirmations.h"

void send_error_message(int socket_descriptor, struct sockaddr_in client_address) {
  unsigned char buffer[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) { buffer[i] = '0'; }
  sendto(socket_descriptor, buffer, sizeof(unsigned char)*BUFFER_SIZE,MSG_CONFIRM, (const struct sockaddr *) &client_address,sizeof(client_address));
}

void send_success_message(int socket_descriptor, struct sockaddr_in client_address) {
  unsigned char buffer[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) { buffer[i] = '1'; }
  sendto(socket_descriptor, buffer, sizeof(unsigned char)*BUFFER_SIZE,MSG_CONFIRM, (const struct sockaddr *) &client_address,sizeof(client_address));
}

// recieve confirmation that operation succeded
_Bool get_confirmation(int socket_descriptor, struct sockaddr_in server_address, int len) {
  unsigned char str[BUFFER_SIZE] = {'\0'};
  struct timeval tv;
  tv.tv_sec = TIMEOUT_S;
  tv.tv_usec =  TIMEOUT_MS * 1000;
  setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));

  recvfrom(socket_descriptor, str, sizeof(unsigned char)*BUFFER_SIZE,MSG_WAITALL, ( struct sockaddr *) &server_address,(unsigned int*)&len);
  _Bool success = 1;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (str[i] != '1') {
       success = 0;
       break;
    }
  }

  if (!success) {
    return 0;
  } else {
    return 1;
  }
}

// recieve last confirmation that operation succeded
_Bool get_last_confirmation(int socket_descriptor, struct sockaddr_in server_address, int len) {
  unsigned char str[BUFFER_SIZE] = {'\0'};
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));

  recvfrom(socket_descriptor, str, sizeof(unsigned char)*BUFFER_SIZE,MSG_WAITALL, ( struct sockaddr *) &server_address,(unsigned int*)&len);
  _Bool success = 1;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (str[i] != '1') {
       success = 0;
       break;
    }
  }

  if (!success) {
    return 0;
  } else {
    return 1;
  }
}
