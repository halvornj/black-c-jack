//
// Created by halvo on 03/11/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "d1_udp.h"

struct Player{
    D1Peer* server;
    int* hand;
    int hand_size;
};
typedef struct Player Player;

Player * create_player();

int delete_player(Player *player);
#endif //PLAYER_H
