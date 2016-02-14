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
+ Connect to nodes.
+ Accept all the packets type.
+ Decrypt/encrypt and sign everything 
+ Calculate pow.

But it cannot:
+ Read/Send broadcast.
+ use a different elliptic curve different from 0x02CA (714).
+ It doesn't have a real system for reading the messages: it just print the messages on console whenever it arrives on network.
+ It doesn't read/decrypt the messages already in the invenctory.
+ Can handle only v4 address and pubkey
