# black-c-jack
This is a fun hobby-project, to create a online multiplayer blackjack-game in c.
The main goal is to create a server/dealer, which runs on a server on a local network on a shared vpn with your friends, and a client that each of them can run that will connect to the server and play blackjack, with the server acting as the dealer.
This means you could play alone if you don't have any friends :D
## Inspiration
The basic inspiration for this comes from observing a blackjack table;
1. When no players are present, the dealer is idle. However, the dealer is always there, ready for any player to sit down.
2. A player could sit down at any moment, at which point an exchange begins between the dealer and the player. This exchange _(splitting, standing, hitting, etc)_ is the game of blackjack being played.
3. More players may come and go from the table entirely independently - any player joining the table, or walking away, should have no impact on the other players (apart from the cards being dealt, of course).
**This has all the defining characteristics of a client/server architecture**, making the game an ideal playground. I'm not creating this project as an exercise in blackjack, but to practice and further my understanding of networking fundamentals and C. 

## future features
- At first I aim for a simple "stateless" implementation where once you disconnect, you're done. It would be nice to later keep a ledger of the `sockaddr`s? of clients and their balance.
- after everything works as a cli, it would be fun to write a ui in C as well

## Rules
Oh, and there are some arbitrary rules I've set for myself. The only resources I'm allowed are
- `man`, meaning the GNU C library manual.
- Code previously written by myself (drawing mostly from my old repos from IN2140 and IN3000)
- Old lecture-slides from any university-course I've taken (I'm including these because, while I might recall something about threading being called `pthread` from a lecture 2 years ago, I've got no chance of remembering that the function I need is `pthread_create()`. And that makes it hard to look up in the manual.)

This means no stack overflow, and *DEFINITELY* no AI assistant. 