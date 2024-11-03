//
// Created by halvo on 03/11/2024.
//

#include <stdio.h>
#include "dealer.h"

int main(){
    Dealer* dealer = create_dealer();
    if (!dealer)
    {
        printf("Failed to create Lookup client.\n");
        return -1;
    }


    return 0;
}