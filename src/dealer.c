//
// Created by halvo on 03/11/2024.
//

#include "dealer.h"
#include <stdio.h>
#include <stdlib.h>
#include "d1_udp.h"


Dealer *create_dealer()
{
    Dealer *dealer = (Dealer *)malloc(sizeof(Dealer));
    if (dealer == NULL)
    {
        perror("malloc");
        return NULL;
    }
    dealer->num_players = 0;


};