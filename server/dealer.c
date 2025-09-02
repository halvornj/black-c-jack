#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "dealer.h"

 
#define MAX_PLAYER_COUNT 10
#define RESHUFFLE_DECK_LIMIT 30

int main(){
  
  struct sockaddr_in serv_addr;
  struct dealer dealer;
  mtx_init(&(dealer.playerll_lock), mtx_plain);
  cnd_init(&(dealer.players_present));
  dealer.table_is_empty = true;
  
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


  pthread_t listening_thread;
  if((pthread_create(&listening_thread, NULL, listen_for_new_player, (void*)&dealer)) != 0){
    fprintf(stderr, "error creating listening thread.\n");
    exit(EXIT_FAILURE);
  }
  printf("spawned listening thread...\n");
  

  /****
   ****MAIN GAMEPLAY LOOP****
  *****/
  
  //TEMP::
  int temp = 0;
  while(true){//main gameplay loop
    //note: we need a separate thread running a separate infinite loop, listening for new players. This means that here we go through a MUTEXED linked list of players who do their turns, and the listening-thread adds new entries to that mutexed list.
    printf("%d\n", temp++);
    sleep(1);
    
    /*
*********
this is commented out, so i can test the multithreading/recieving of clients. I'm just gonna run main as an infinite loop printing numbers and sleeping, and we'll see if the listening-thread prints some good stuff. 

    mtx_lock(&(dealer.playerll_lock));
    while(dealer.table_is_empty){
      cnd_wait(&(dealer.players_present), &(dealer.playerll_lock));
    }
    //guaranteed we now have at least one player, and that we are holding playerll_lock? according to how i remember condition vars
    dealer.current_player = dealer.current_player->next;
    if(dealer.current_player == dealer.head_player){//we've gone back around to the start of the table, this means we collect the cards and deal.

      //TODO before we shuffle, we need to play the dealers hand, and pay out.
     
      //MAKING A BIG COMMENT, BECAUSE THAT ^ IS REALLY IMPORTANT! MUST GO BEFORE DEALING, OBV.
       
     
      if(SHOE_SIZE - dealer.cards_dealt < RESHUFFLE_DECK_LIMIT){
	reshuffle_deck(&dealer);
	dealer.cards_dealt = 0;
      }
      struct player* dealing_to = dealer.head_player;
      bzero((void*) &(dealing_to->hand), (sizeof(card_t) * MAX_CARDS_PER_HAND)); //empty the hand
      dealing_to->hand[0] = deal_card(&dealer);
      dealing_to->hand[1] = deal_card(&dealer);
      dealing_to = dealing_to->next;
      //we've dealt to the first player, now we go around and deal to everyone else.
      while(dealing_to != dealer.head_player){
	bzero((void*) &(dealing_to->hand), (sizeof(card_t) * MAX_CARDS_PER_HAND));
	dealing_to->hand[0] = deal_card(&dealer);
	dealing_to->hand[1] = deal_card(&dealer);
	dealing_to = dealing_to->next;
      }
    }
    //all players should now have 2 cards.
    //where the hell do i unlock the ll???? here I guess
    mtx_unlock(&(dealer.playerll_lock));

    
    

    dealer.current_player = dealer.current_player->next; //advance player. Note, this is at the end of the while().

    **********
    */

   
      
  }
  

  
  return EXIT_SUCCESS;
}

/*
this function should be called with an already randomized deck. If the deck is not random, this deals them in order.
 */
card_t* deal_card(struct dealer* dealer){
  return dealer->deck[dealer->cards_dealt++];
}


void reshuffle_deck(struct dealer* dealer){
  //randomizing...:
  //initialize with [2D,2D,2D,2D,2H,2H,2H,2H,2C,2C....] (for 4 decks), then loop over, and for each, choose random index and swap?
  //start with sorted deck:

  //TODO extract 13 and 4 to defines/consts
  int current_card_idx = 0;
  char nums[13] = {'2','3','4','5','6','7','8','9','t','j','q','k','a'};
  char suits[4]={'D', 'H', 'C', 'S'};
  for(int i=0; i<NUM_DECKS; i++){
    for(int suit_idx = 0; suit_idx < 4; suit_idx++){
      for(int num_idx = 0; num_idx < 13; num_idx++){
	card_t card = {nums[num_idx], suits[suit_idx]};
	dealer->deck[current_card_idx++] = &card;
      }
    }
  }
  //deck should now contain all cards, now we loop over and swap every card for another random card. This should be random?
  for(int idx = 0; idx<SHOE_SIZE; idx++){
    int swap_idx = (random() % SHOE_SIZE);
    card_t* temp = dealer->deck[swap_idx];
    dealer->deck[swap_idx] = dealer->deck[idx];
    dealer->deck[idx] = temp;
  }
  printf("deck randomized.\n");
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
    int cli_addr_len = sizeof(client_sockaddr); //note; this could also be saved, to check and restore balance on reconnect between sessions?
    //accept new connection,
    new_player.socket_fd = accept(dealer->serv_socket_fd, (struct sockaddr*) &client_sockaddr, (socklen_t*)&cli_addr_len);
    if(new_player.socket_fd <0){
      fprintf(stderr, "error accepting connection.\n");
      exit(EXIT_FAILURE);
    }
    printf("(listening thread)  accepting connection...\n");
    
    //inserting new player
    mtx_lock(&(dealer->playerll_lock));
    if(dealer->head_player == NULL){//no players present, this connection is the first
      new_player.next = &new_player;
      new_player.prev = &new_player; //ring-buffer
      dealer->current_player = &new_player;
      dealer->head_player = &new_player;
      
    }else{ //insert at the end
      new_player.next = dealer->head_player;
      new_player.prev = dealer->head_player->prev;
      dealer->head_player->prev->next = &new_player;
      dealer->head_player->prev = &new_player;
      //note; we dont set a .new-bool here, but when we revolve players in the gameplay loop we must check if they have cards. if the "current player" does not have any cards, it means they joined mid-round, and should be skipped.
    }
    cnd_signal(&(dealer->players_present)); //signal that there are players present, and gameloop may run.
    mtx_unlock(&(dealer->playerll_lock));
  }
}
