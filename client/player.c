#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>
#include "player.h"


int main(){
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  if(socket_fd ==-1){
    fprintf(stderr, "failure creating client socket.\n");
    exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  addr.sin_port = htons(SERVER_PORT);

  if((connect(socket_fd, (struct sockaddr *) &addr, sizeof(addr))) != 0){
    fprintf(stderr, "failure binding client socket.\n");
    fprintf(stderr, "errno: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("successfully connected to dealer.\n");
  


  return EXIT_SUCCESS;
}
