//
// Created by halvo on 03/11/2024.
//

#include "dealer.h"
#include <stdio.h>
#include <stdlib.h>
#include "d1_udp.h"
#include "player.h"

#define SERVER_PORT 2311

Dealer *create_dealer()
{
    Dealer *dealer = (Dealer *)malloc(sizeof(Dealer));
    if (dealer == NULL)
    {
        perror("malloc");
        free(dealer);
        return NULL;
    }
    dealer->num_players = 0;
    dealer->players = NULL;

    // Initialize the dealer main port/file descriptor, that is used to recieve players
    dealer -> peer = d1_create_client();
    if (dealer -> peer == NULL)
    {
        perror("d1_create_client");
        free(dealer);
        return NULL;
    }

    //reusing old code:
    if (!d1_get_peer_info(dealer->peer, "localhost", SERVER_PORT)) //temporarily set the server to localhost
    {
        perror("d1_get_peer_info");
        free(dealer);
        return NULL;
    }
    dealer->peer->addr.sin_addr.s_addr = htonl(INADDR_ANY); //set the address to be any address

    //binding the socket to the address
    bind(dealer->peer->socket, (struct sockaddr *)&dealer->peer->addr, sizeof(dealer->peer->addr));

    return dealer;
}


int delete_dealer(Dealer *dealer)
{
    if (dealer == NULL)
    {
        return -1;
    }

    // Close the dealer's main port/file descriptor
    d1_delete(dealer->peer);

    // Free the dealer's player list
    for (int i = 0; i < dealer->num_players; i++)
    {
        d1_delete(dealer->players[i]);
    }
    free(dealer->players);

    // Free the dealer
    free(dealer);

    return 0;
}

int add_player(Dealer *dealer, D1Peer *player)
{
    if (dealer == NULL || player == NULL)
    {
        return -1;
    }

    dealer->num_players++;
    dealer->players = (D1Peer **)realloc(dealer->players, dealer->num_players * sizeof(Player *));
    dealer->players[dealer->num_players - 1] = player;
    return 0;
}



int remove_player(Dealer *dealer, D1Peer *player)
{
   //TODO: Implement this function
    return 0;
}

int main()
{
    char buffer[1024];
    int bufflen = 1024;
    Dealer *dealer = create_dealer();
    if (!dealer)
    {
        printf("Failed to create Lookup client.\n");
        free(dealer);
        return -1;
    }
    int rc = d1_recv_data(dealer->peer, buffer, bufflen);
    if (rc < 0)
    {
        perror("d1_recv_data");
        free(dealer);
        return -1;
    }
    printf("Received %d bytes\n", rc);


    delete_dealer(dealer);
    return 0;
}