CFLAGS=-g -std=gnu11 -Wall -Wextra
LDFLAGS=-g

all: dealer

dealer: dealer.o
	gcc $(LDFLAGS) -o $@ $^


%.o: %.c
	gcc $(CFLAGS) -c $^

clean:
	rm -f *.o
	rm -f server
