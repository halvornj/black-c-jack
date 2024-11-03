//
// Created by halvo on 03/11/2024.
//
#include <stdio.h>
#include "player.h"

#include <stdlib.h>

#include "dealer.h"
#include "d1_udp.h"

/*
 *creates a player connected to the hardwired HAMACHI server, returns NULL on failure.
 *
 */
Player * create_player()
{
    Player *player = (Player *)malloc(sizeof(Player));
    if (player == NULL)
    {
        perror("malloc");
        return NULL;
    }

    player->server = d1_create_client();
    if (player->server == NULL)
    {
        perror("d1_create_client");
        free(player);
        return NULL;
    }
    if(d1_get_peer_info(player->server, "25.46.25.156",2311)==0)
    {
        d1_delete(player->server);
        free(player);
        return NULL;
    }
    return player;
}

int delete_player(Player *player)
{
    if (player != NULL)
    {
        d1_delete(player->server);
        free(player);
    }
    return EXIT_SUCCESS;
}
