CFLAGS=-g -std=gnu11 -Wall -Wextra
LDFLAGS=-g

all: player

player: player.o
	gcc $(LDFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $^

clean:
	rm -f *.o
	rm -f player
