#ifndef DEALER_H
#define KERNEL_H

#include <stdbool.h>
#define MAX_CARDS_PER_HAND 21 // if there is an infinite shoe, you could get 21 aces. 

typedef char card_t[2]; //each card is represened as 2 characters, e.g. 2D for 2 of diamods, kH for king of hearts

struct player {
  int socket_fd;
  uint16_t balance;
  card_t hand[MAX_CARDS_PER_HAND];
  bool split;
  card_t split_hand[MAX_CARDS_PER_HAND];
}

/*prototypes*/
//  int listen_for_new_player(); // will need a pointer to the player-ll, and a mutex.
  

#endif
