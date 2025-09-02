#ifndef COMMON_H
#define COMMON_H


#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 2311
#define MSG_BUFSZ 3 //message is 1 byte type, and potentially 2 bytes card, 2 bytes bet/balance or 1 byte action


typedef char card_t[2]; //each card is represened as 2 characters, e.g. 2D for 2 of diamods, kH for king of hearts


//message structs/types
enum message_type {
  MSG_DC,
  MSG_CARD,
  MSG_ACTION,
  MSG_BET
};

enum action{
  ACT_HIT,
  ACT_STAND,
  ACT_DOUBLE,
  ACT_SPLIT
};

struct msg_header{
  enum message_type type; //one of message_type
};

struct msg_action{
  enum message_type type; //one of message_type
  enum action action;
};
struct msg_bet{
  enum message_type type; //one of message_type
  uint16_t amount;
};
struct msg_card{
  enum message_type type; //one of message_type
  card_t card;
};


#endif
