#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include "dealer.h"

 
#define SERVER_PORT 2311
#define MAX_PLAYER_COUNT 10

int main(){
  
  int sock_fd;
  struct sockaddr_in serv_addr;

  
  sock_fd = socket(AF_INET, SOCK_STREAM, 0); //ipv4, TCP/STREAM, default protocol
  if(sock_fd == -1){
    fprintf( stderr, "failed creating socket\n");
    exit(EXIT_FAILURE);
  }
  printf("socket created..\n");

  //init sockadr
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if(bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0){
    fprintf(stderr,"failure binding socket\n");
    exit(EXIT_FAILURE);
  }


  while(true){//main gameplay loop
    //note: we need a separate thread running a separate infinite loop, listening for new players. This means that here we go through a MUTEXED linked list of players who do their turns, and the listening-thread adds new entries to that mutexed list.

  
  }
  

  
  return EXIT_SUCCESS;
}

//this is the function passed to the thread, that listens and accepts new players.
int listen_for_new_player(struct* dealer){
  while (true){ //this thread should run forever, listening and adding.
    //init empty player to be added
    struct player new_player;
    new_player.socket_fd = -1;
    new_player.balance = 500;
    //todo maybe init rest

    //set dealer to listen,
    if((listen(dealer->serv_socket_fd, MAX_PLAYER_COUNT)) != 0){
      fprintf(stderr, "failure setting socket to listen.\n");
      exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_sockaddr;
    //accept new connection,
    new_player.socket_fd = accept(dealer->serv_socket_fd, (sockaddr*) &client_sockaddr, sizeof(client_sockaddr));
    if(new_player.socket_fd <0){
      fprintf(stderr, "error accepting connection.\n");
      exit(EXIT_FAILURE);
    }
    printf("accepting connection...\n");

    
    
  }
}
