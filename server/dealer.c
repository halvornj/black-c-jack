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
  
  struct sockaddr_in serv_addr;
  struct dealer dealer;
  

  
  dealer.serv_socket_fd = socket(AF_INET, SOCK_STREAM, 0); //ipv4, TCP/STREAM, default protocol
  if(dealer.serv_socket_fd == -1){
    fprintf( stderr, "failed creating socket\n");
    exit(EXIT_FAILURE);
  }
  printf("socket created..\n");

  //init sockadr
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if(bind(dealer.serv_socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0){
    fprintf(stderr,"failure binding socket\n");
    exit(EXIT_FAILURE);
  }

  //TODO start a thread running listen_for_new_player()
  pthread_t listening_thread;
  if((pthread_create(&listening_thread, NULL, listen_for_new_player, (void*)&dealer)) != 0){
    fprintf(stderr, "error creating listening thread.\n");
    exit(EXIT_FAILURE);
  }
  printf("spawned listening thread...\n");
  
  

  
  /*
  while(true){//main gameplay loop
    //note: we need a separate thread running a separate infinite loop, listening for new players. This means that here we go through a MUTEXED linked list of players who do their turns, and the listening-thread adds new entries to that mutexed list.
  
  }
  */
  

  
  return EXIT_SUCCESS;
}

//this is the function passed to the thread, that listens and accepts new players.
void* listen_for_new_player(void* args){
  struct dealer* dealer = (struct dealer*) args;
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
    int cli_addr_len = sizeof(client_sockaddr); //note; this could also be saved in the player, to check and restore balance on reconnect between sessions?
    //accept new connection,
    new_player.socket_fd = accept(dealer->serv_socket_fd, (struct sockaddr*) &client_sockaddr, (socklen_t*)&cli_addr_len);
    if(new_player.socket_fd <0){
      fprintf(stderr, "error accepting connection.\n");
      exit(EXIT_FAILURE);
    }
    printf("(listening thread)  accepting connection...\n");

    //inserting new player
    if(dealer->head_player == NULL){//no players present, this connection is the first
      new_player.next = &new_player;
      new_player.prev = &new_player; //ring-buffer

      mtx_lock(&(dealer->playerll_lock));//we need a mutex to manipulate the ll
      dealer->current_player = &new_player;
      dealer->head_player = &new_player;
      mtx_unlock(&(dealer->playerll_lock));
    }else{ //insert at the end
      new_player.next = dealer->head_player;
      new_player.prev = dealer->head_player->prev;

      mtx_lock(&(dealer->playerll_lock));
      dealer->head_player->prev->next = &new_player;
      dealer->head_player->prev = &new_player;
      mtx_unlock(&(dealer->playerll_lock));
      //note; we dont set a .new-bool here, but when we revolve players in the gameplay loop we must check if they have cards. if the "current player" does not have any cards, it means they joined mid-round, and should be skipped.
    }
    cnd_signal(&(dealer->players_present)); //signal that there are players present, and gameloop may run.
  }
}
