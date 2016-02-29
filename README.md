# BitMRC
A simple bitmessage client implementation written in c++

It compile on windows using visual studio, but i think it can be easily ported to linux by changing the windows socket.

It uses crypto++ for cryptography so i included it.

At this stage it can:
+ Read/send messages.
+ Read/send pubkeys.
+ Save/load private addresses.
+ Save/load blocks.
+ Generate random addresses.
+ Generate deterministic addresses.
+ Connect to nodes.
+ Accept all the packets type.
+ Decrypt/encrypt and sign everything 
+ Calculate pow.
+ It does propagate inventory elements
+ There is a random delay when propagating inv different from node to node (0.1 to 2.1 sec) this should prevent some minor attacks

But it cannot:
+ Read/Send broadcast.
+ use a different elliptic curve different from 0x02CA (714).
+ It doesn't read/decrypt the messages already in the invenctory.
+ Can handle only v4 address and pubkey
+ It doesn't use ack at all
