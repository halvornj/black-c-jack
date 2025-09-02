#ifndef DEALER_H
#define KERNEL_H

#include <stdbool.h>
#include <threads.h>
#include "../common.h"

#define MAX_CARDS_PER_HAND 21 // if there is an infinite shoe, you could get 21 aces. 
#define NUM_DECKS 4
#define SHOE_SIZE 208 //note that this is dependent on NUM_DECKS, but i dont want to insert a bunch of unnecessary multiplications.

struct player {
  int socket_fd;
  uint16_t balance;
  card_t* hand[MAX_CARDS_PER_HAND];
  uint8_t current_score;
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
//TODO docs
void* listen_for_new_player(void* args); //args is actually a pointer to a dealer
void reshuffle_deck(struct dealer* dealer);
card_t* deal_card(struct dealer* dealer);
void* play_hand(void* args); //args should be a pointer to dealer. 
uint8_t card_score(card_t* card);
#endif
