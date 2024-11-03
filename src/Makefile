CFLAGS=-g -std=gnu11 -Wall -Wextra
LDFLAGS=-g

all: dealer player

libhe.a: d1_udp.o
	ar rc $@ $^

player: player.o libhe.a
	gcc $(LDFLAGS) -o $@ $^


dealer: dealer.o libhe.a
	gcc $(LDFLAGS) -o $@ $^

d1_udp_client.o: d1_udp.c d1_udp.h d1_udp_mod.h

d1_udp.o: d1_udp.c d1_udp.h d1_udp_mod.h

%.o: %.c
	gcc $(CFLAGS) -c $^

clean:
	rm -f d1_udp_client *.o *.gch

