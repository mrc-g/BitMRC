# BitMRC
A simple bitmessage client implementation written in c++

It uses crypto++ for cryptography so i included it.

For discussion we have a bitmessage chan:

Name: BitMRC-Development-Talk

Address: BM-2cVxGMPpzu1WwnpUwAvcy9aQpuS9deouky


##### Compile:
###### Linux:
```
#cmake .
#make
```
if not working because of linking try ./doMakeLinux.sh

###### Windows:

compile with Visual Studio

At this stage it can:
+ Read/send messages.
+ Read/send broadcasts.
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
+ use a different elliptic curve different from 0x02CA (714).
+ It doesn't read/decrypt the messages already in the invenctory.
+ Can handle only v4 address and pubkey
+ It only resend ack but it does not use it
+ It doesn't support ipv6
