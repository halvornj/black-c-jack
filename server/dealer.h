#ifndef DEALER_H
#define KERNEL_H

#include <stdbool.h>
#include <threads.h>
#define MAX_CARDS_PER_HAND 21 // if there is an infinite shoe, you could get 21 aces. 
#define NUM_DECKS 4
#define SHOE_SIZE 208 //note that this is dependent on NUM_DECKS, but i dont want to insert a bunch of unnecessary multiplications.
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 2311

typedef char card_t[2]; //each card is represened as 2 characters, e.g. 2D for 2 of diamods, kH for king of hearts


struct player {
  int socket_fd;
  uint16_t balance;
  card_t* hand[MAX_CARDS_PER_HAND];
  bool split;
  card_t* split_hand[MAX_CARDS_PER_HAND];
  struct player* prev; //for the ll. I think we do this as a ring-buffer? not sure
  struct player* next;
};
  
struct dealer{
  int serv_socket_fd;
  struct player* current_player;
  struct player* head_player;
  //locks for manipluating players:
  cnd_t players_present;
  mtx_t playerll_lock;
  bool table_is_empty;

  card_t* deck[SHOE_SIZE];
  uint8_t cards_dealt;

  card_t* hand[MAX_CARDS_PER_HAND];
};
 
/*prototypes*/
void* listen_for_new_player(void* args); //args is actually a pointer to a dealer
void reshuffle_deck(struct dealer* dealer);
card_t* deal_card(struct dealer* dealer);


#endif
