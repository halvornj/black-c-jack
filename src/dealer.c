//
// Created by halvo on 03/11/2024.
//

#include "dealer.h"
#include <stdio.h>
#include <stdlib.h>
#include "d1_udp.h"


int main(int argc, char *argv[]){

    if (argc < 3)
    {
        fprintf(stderr, "Usage %s <host> <port>\n"
                        "    <host> - name of the server. Can be localhost, a regular name, or an address in dotted decimal.\n"
                        "    <port> - UDP port the server uses for listening.\n"
                        "\n",
                argv[0]);
        return -1;
    }
    char *server_name = argv[1];
    uint16_t server_port = atoi(argv[2]);

    /* Create a suitable data structure to manage the assocation of this client
     * with a server.
     */
    D1Peer *client = d1_create_client();
    if (!client)
    {
        printf("Failed to create D1 client.\n");
        return -1;
    }
}