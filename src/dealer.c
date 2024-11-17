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
    reset_dealer_port(dealer);

    //binding the socket to the address
    bind(dealer->peer->socket, (struct sockaddr *)&dealer->peer->addr, sizeof(dealer->peer->addr));

    return dealer;
}

int reset_dealer_port(Dealer *dealer)
{
    //reusing old code:
    if (!d1_get_peer_info(dealer->peer, "localhost", SERVER_PORT)) //temporarily set the server to localhost
    {
        perror("d1_get_peer_info");
        free(dealer);
        return -1;
    }
    dealer->peer->addr.sin_addr.s_addr = htonl(INADDR_ANY); //set the address to be any address
    return 0;
}


int delete_dealer(Dealer *dealer)
{
    if (dealer == NULL)
    {
        return -1;
    }

    // Close the dealer's main port/file descriptor
    d1_delete(dealer->peer);


    free(dealer->players);

    // Free the dealer
    free(dealer);

    return 0;
}

int gameloop(Dealer *dealer)
{
    char buffer[512];
    int bufflen = 512;
    Client* client = d1_recv_data(dealer->peer, buffer, bufflen);
    if (client == NULL)
    {
        fprintf(stderr, "error in d1_recv_data: client was NULL\n");
        return -1;
    }

    int found = 0;
    for(int i = 0; i<dealer->num_players; i++)
    {
        if(dealer->players[i] == client)
        {
            found = i;
            break;
        }
    }         //lazy way to check if the client is already in the list

    if(client->addr.sin_addr.s_addr == dealer->next_player->addr.sin_addr.s_addr) //if the client that sent us a message is the expected next player:
    {
        //do stuff

        //finally, advance to the next player
        dealer->next_player = dealer->players[found+1%dealer->num_players]; //set the next player to be the next player in the list
    }
    else
    {
        if(!found) //if they weren't already connected, add them to the list.
        {
            dealer->players = realloc(dealer->players, sizeof(Client) * (dealer->num_players + 1));
            dealer->players[dealer->num_players] = client;
            dealer->num_players++;
            //TODO: send a message to the client that they have been added to the game
        }else
        {
            //TODO: send a message to the client that they are already in the game, and that they should wait for their turn
        }

    }

    //finally, open the socket to allow anyone to message
    reset_dealer_port(dealer);
    return gameloop(dealer);
}





int main()
{
    Dealer *dealer = create_dealer();
    if (!dealer)
    {
        printf("Failed to create Lookup client.\n");
        free(dealer);
        return -1;
    }

    delete_dealer(dealer);
    return 0;
}