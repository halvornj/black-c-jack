#ifndef DEALER_H
#define KERNEL_H

#include <stdbool.h>
#include <threads.h>
#include "../common.h"

#define NUM_DECKS 4
#define SHOE_SIZE 208 //note that this is dependent on NUM_DECKS, but i dont want to insert a bunch of unnecessary multiplications.
#define STARTING_BALANCE 500

struct client {
  int socket_fd;
  struct player* player;
  struct client* prev; //for the ll. I think we do this as a ring-buffer? not sure
  struct client* next;
};
  
struct dealer{
  int serv_socket_fd;
  struct client* current_player;
  struct client* head_player;
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
void deal_card(struct dealer* dealer);
void play_hand(void* args); //args should be a pointer to dealer. 
uint8_t card_score(card_t* card);
void remove_current_player(struct dealer* dealer);
void send_game_state(struct dealer* dealer);
#endif
