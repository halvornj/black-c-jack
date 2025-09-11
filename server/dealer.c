#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
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
  
  while(true){//main gameplay loop
    //note: we need a separate thread running a separate infinite loop, listening for new players. This means that here we go through a MUTEXED linked list of players who do their turns, and the listening-thread adds new entries to that mutexed list.
    

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
      
      bzero((void*) &(dealer.current_player->player->hand), (sizeof(card_t) * MAX_CARDS_PER_HAND)); //empty the hand
      deal_card(&dealer);
      deal_card(&dealer);
      dealer.current_player = dealer.current_player->next;
      //we've dealt to the first player, now we go around and deal to everyone else.
      while(dealer.current_player != dealer.head_player){
	bzero((void*) &(dealer.current_player->player->hand), (sizeof(card_t) * MAX_CARDS_PER_HAND));
	deal_card(&dealer);
	deal_card(&dealer);
	dealer.current_player = dealer.current_player->next;
      }
    }
    //all players should now have 2 cards.
    //where the hell do i unlock the ll???? here I guess
    mtx_unlock(&(dealer.playerll_lock));

    /* (Imagine there is oceans 11 heist music playing) here's the plan:
       1.We spawn one cancelable thread, P, which does the play-communication / back-and-forth with the current player. recieving their moves, sending them cards. This thread will naturally exit either when the player stands, or goes bust.
       2. We start some sort of timer, maybe on another thread T, which upon completion, cancels P. This is the timeout, if you spend more than 30 seconds? on a hand.
       3. on main-thread, we wait/join on P. This means that once we've joined, we know that their turn is over - either from a time out, a stand or a bust. We can then move on to the next player.
     */

    
    

    
    

    dealer.current_player = dealer.current_player->next; //advance player. Note, this is at the end of the while().
      
  }
  

  
  return EXIT_SUCCESS;
}

/*
 * This is the function that interacts with the current player. It should be passed to a cancellable thread, which gets cancelled on a timeout. This function/thread returns when either the player stands, or goes bust.
 * @args: a void-cast of a pointer to the dealer. formatted as `void* args` begause pthread wants that and I'm lazy.
 @returns: nothing, but upon thread-completion (from exit or cancel), a hand is done. The next player can be handled.
 */
void play_hand(void* args){
  struct dealer* dealer = (struct dealer*) args;
  struct client* cur_pl = dealer->current_player;
  //notes; keep man pages for pthread_cancel() and pthread_setcancelstate() open.
  //I'm thinking we disable cancels while we draw and send cards, then enable before we listen for a response.
  // not actually sure we need to disable cancels as it is only at end of turn anyway, so it doesnt matter if we draw a card and get cancelled before sending. The timer would be over, so it doesn't matter?
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); //should be default, but just in case.
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  //okay, now for the actual blackjack stuff :(
  //the ace is going to make this a real pain >:(
  cur_pl->player->current_score = card_score(cur_pl->player->hand[0]) + card_score(cur_pl->player->hand[1]);

  //first, we tell player that it is their turn.
  //TODO
  
  while(cur_pl->player->current_score < 22){ //keep in mind, we get cancelled by a timer if we take too long
    //and we return/break if we stand.

    uint8_t buf[MSG_BUFSZ];
    int rc = recv(cur_pl->socket_fd, &buf, MSG_BUFSZ, 0);
    if(rc<1){ //we didn't recieve anything
      fprintf(stderr, "recieved malformed message...\n");
    }
    struct msg_header headr = *((struct msg_header*) &buf);

    if(headr.type == MSG_DC){
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      remove_current_player(dealer);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); //doesn't matter, we return anyway.
      return;
    }

    if(headr.type == MSG_CARD){
      fprintf(stderr, "error: client sent card to server.\n");
      return;
    }

    
    
  } 
}


void remove_current_player(struct dealer* dealer){
  mtx_lock(&(dealer->playerll_lock)); // we need to lock the ll for removal.

  dealer->current_player->prev->next = dealer->current_player->next;
  dealer->current_player->next->prev = dealer->current_player->prev;

  free(dealer->current_player->player);
  free(dealer->current_player);

  mtx_unlock(&(dealer->playerll_lock));
}


/*returns the value for a card. Counts aces as 11, it is up to bust-handler to retreat as a 1 and subtract 10 from total score*/
uint8_t card_score(card_t* card){
  if(isdigit((*card)[0])){//not a face card.
    return ((*card)[0] - '0');
  }
  if((*card)[0]=='a'){
    return 11;
  }
  return 10; //face card, but not ace.
}




/*
 * this function selects a random card from the dealers deck, and sends it to all clients.
 @param dealer: a pointer to the dealer
 @returns: nothing, but will send messages to all players about the newly dealt cards
 */
void deal_card(struct dealer* dealer){
  card_t* card = dealer->deck[dealer->cards_dealt++];

  struct msg_card msg;
  msg.type = MSG_CARD;
  msg.card = *card;
  msg.name = dealer->current_player->player->name;
  
  
  struct client* current_cli = dealer->head_player;
  int rc;
  //send to current player,
  rc = send(current_cli->socket_fd, (void *) &msg, sizeof(msg), 0);
  if(rc<0){
    fprintf(stderr, "error sending card to player...\n");
    exit(EXIT_FAILURE);
  }

  //send as status for all players
  current_cli = current_cli->next;

  while(current_cli != dealer->current_player){
    rc = send(current_cli->socket_fd, (void *) &msg, sizeof(msg), 0);
    if(rc<0){
      fprintf(stderr, "error sending card to player...\n");
      exit(EXIT_FAILURE);
    }
    
    current_cli = current_cli->next;
  }

  
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
    struct client client;
    client.socket_fd = -1;

    //set dealer to listen,
    if((listen(dealer->serv_socket_fd, MAX_PLAYER_COUNT)) != 0){
      fprintf(stderr, "failure setting socket to listen.\n");
      exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_sockaddr;
    int cli_addr_len = sizeof(client_sockaddr);
    
    //accept new connection,
    client.socket_fd = accept(dealer->serv_socket_fd, (struct sockaddr*) &client_sockaddr, (socklen_t*)&cli_addr_len);
    if(client.socket_fd <0){
      fprintf(stderr, "error accepting connection.\n");
      exit(EXIT_FAILURE);
    }

    //allocate player-struct
    client.player = malloc(sizeof(struct player));
    
    //receive their name.
    int rc = recv(client.socket_fd, (void *) &(client.player->name),MAX_NAME_LENGTH, 0);
    if(rc<0){
      fprintf(stderr, "error recieving player name.\n");
    }

    client.player->balance = STARTING_BALANCE;
    
    printf("(listening thread)  accepting connection from %s...\n", client.player->name);    

    
    //inserting new player
    mtx_lock(&(dealer->playerll_lock));
    
    if(dealer->head_player == NULL){//no players present, this connection is the first
      client.next = &client;
      client.prev = &client; //ring-buffer
      dealer->current_player = &client;
      dealer->head_player = &client;
      
    }else{ //insert at the end
      client.next = dealer->head_player;
      client.prev = dealer->head_player->prev;
      dealer->head_player->prev->next = &client;
      dealer->head_player->prev = &client;
      //note; we dont set a .new-bool here, but when we revolve players in the gameplay loop we must check if they have cards. if the "current player" does not have any cards, it means they joined mid-round, and should be skipped.
    }
    cnd_signal(&(dealer->players_present)); //signal that there are players present, and gameloop may run.
    mtx_unlock(&(dealer->playerll_lock));
  }
}

void send_game_state(struct dealer* dealer){
  if(dealer->head_player == NULL){
    fprintf(stderr,"error: sending state to empty table. aborting...\n");
    exit(EXIT_FAILURE);
  }

  //send update to head
  
  //advance current

  //loop while current not head
  
}
