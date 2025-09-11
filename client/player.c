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

  char name[MAX_NAME_LENGTH];
  char* namebuf;
  unsigned long len;
  printf("please enter a name between 1 and 19 characters: ");
  int read = getline(&namebuf, &len, stdin);
  while(read < 2 || read>MAX_NAME_LENGTH){ //2, because we always have a newline?
    printf("error: name is not allowed. Please enter a name between 1 and 19 characters: ");
    read =getline(&namebuf, &len, stdin);
  }
  namebuf[strlen(namebuf)-1] = '\0';
  memcpy(name, namebuf, MAX_NAME_LENGTH);

  free(namebuf);

  
  addr.sin_family = AF_INET;
  //todo change this to use inet_aton()
  addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  addr.sin_port = htons(SERVER_PORT);

  if((connect(socket_fd, (struct sockaddr *) &addr, sizeof(addr))) != 0){
    fprintf(stderr, "failure binding client socket.\n");
    fprintf(stderr, "errno: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("successfully connected to dealer.\n");

  //sending player name.
  int rc = send(socket_fd, name, MAX_NAME_LENGTH, 0);
  if(rc < 0){
    fprintf(stderr, "error sending player name to server.\n");}
  


  return EXIT_SUCCESS;
}
