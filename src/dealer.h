#ifndef DEALER_H
#define DEALER_H

#include "d1_udp.h"

struct Dealer
{
    int num_players;
    D1Peer** players;
    D1Peer* peer;
};
typedef struct Dealer Dealer;

/*TODO docs*/
Dealer *create_dealer();

int delete_dealer(Dealer *dealer);

/* At this layer, only the following types of packets are known:
 */
#define TYPE_CONNECT    (1 << 0) /* type is PacketRequest */
#define TYPE_DISCONNECT (1 << 1) /* type is PacketResponseSize */
#define TYPE_DATA            (1 << 2) /* type is PacketResponse */


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
