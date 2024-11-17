#ifndef DEALER_H
#define DEALER_H

#include "d1_udp.h"

struct Client
{
    struct sockaddr_in addr;
    socklen_t addrlen;
    int seqno;
};
typedef struct Client Client;

struct Dealer
{
    Client* next_player;
    int num_players;
    Client **players;
    D1Peer* peer;
};
typedef struct Dealer Dealer;



/*TODO docs*/
struct Dealer* create_dealer();

int delete_dealer(Dealer *dealer);

int reset_dealer_port(Dealer *dealer);


/* At this layer, only the following types of packets are known:
 */
#define TYPE_CONNECT    (1 << 0) /* type is ConnectPacket */
#define TYPE_DISCONNECT (1 << 1) /* type is DisconnectPacket */
#define TYPE_DATA            (1 << 2) /* type is DataPacket */


struct PacketHeader
{
    uint8_t type;
};
typedef struct PacketHeader PacketHeader;

struct ConnectPacket
{
    uint8_t type;

};
typedef struct ConnectPacket ConnectPacket;

struct DisconnectPacket
{
    uint8_t type;
};
typedef struct DisconnectPacket DisconnectPacket;

struct DataPacket
{
    uint8_t type;
    uint16_t payload_size;
};
typedef struct DataPacket DataPacket;


#endif //DEALER_H
