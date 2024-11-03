//
// Created by halvo on 03/11/2024.
//
#include <stdio.h>
#include "player.h"

#include <stdlib.h>

#include "dealer.h"
#include "d1_udp.h"

#define SERVER_PORT 2311
#define SERVER_ADDRESS "localhost" //"25.46.25.156"

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
    if(d1_get_peer_info(player->server, SERVER_ADDRESS,SERVER_PORT)==0)
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

int main()
{
    Player * player = create_player();
    if (!player)
    {
        printf("Failed to create Lookup client.\n");
        free(player);
        return -1;
    }

    int rc = d1_send_data(player->server, "Hello", 5);
    if (rc < 0)
    {
        perror("d1_send_data");
        delete_player(player);
        return -1;
    }
    printf("Sent %d bytes\n", rc);
    delete_player(player);
    return 0;
}