#ifndef COMMON_H
#define COMMON_H


#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 2311
#define MSG_BUFSZ 128 //TODO calculate this
#define MAX_NAME_LENGTH 20
#define MAX_CARDS_PER_HAND 21 // if there is an infinite shoe, you could get 21 aces.
#define HEADER_SIZE sizeof(struct msg_header)

typedef char card_t[2]; //each card is represened as 2 characters, e.g. 2D for 2 of diamods, kH for king of hearts


struct player {
  char name[MAX_NAME_LENGTH];
  uint16_t balance;
  card_t* hand[MAX_CARDS_PER_HAND];
  uint8_t current_score;
  //bool split;
  //TODO split handling. I think we start with blackjack without splitting, then implement later.
  
};

//message structs/types
enum message_type {
  MSG_DC, //message sent by client, saying it is disconnecting.
  MSG_CARD, // message sent by server to client, containing a card.
  MSG_ACTION, //message from client to server, containing a play-action
  MSG_BET, //message from client to server, containing a bet-amount.
  MSG_YOURTURN, //message from server to client, saying it is their turn.
  MSG_INFO,
  MSG_REJECTED //used by server to tell client last message was not accepted.
};
enum action{
  ACT_HIT,
  ACT_STAND,
  ACT_DOUBLE,
  ACT_SPLIT
};

struct msg_header{
  enum message_type type; //one of message_type
  uint16_t size;
};

struct msg_action{
  enum message_type type; //one of message_type
  uint16_t size;
  enum action action;
};
struct msg_bet{
  enum message_type type; //one of message_type
  uint16_t size;
  uint16_t amount;
};
struct msg_card{
  enum message_type type; //one of message_type
  uint16_t size;
  card_t card;
  char name[MAX_NAME_LENGTH];
};
struct msg_info{
  enum message_type type;
  uint16_t size;
}; //todo


#endif
