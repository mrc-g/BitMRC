# BitMRC
A simple bitmessage client implementation written in c++

It uses crypto++ for cryptography so i included the submodule.

For discussion we have a bitmessage chan:

Name: BitMRC-Development-Talk

Address: BM-2cVxGMPpzu1WwnpUwAvcy9aQpuS9deouky


##### Compile:
###### Linux:
```
# cmake . -DENABLE_TESTS
(if you want the tests to be compiled)
# cmake . #
(otherwise)

# make
```
install packages :
- cmake >= 2.8.11
  see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04
- g++ >= 4.9
  see http://askubuntu.com/questions/428198/getting-installing-gcc-g-4-9-on-ubuntu for ubuntu/mint etc.
- libsqlite3-dev
- libvsqlitepp3
- sqlite3
- libcpptest-dev

###### Windows (Visual Studio):
open command prompt
```
git submodule init
git submodule update
```
then compile with visual studio and convert if necessary

##### Functionality:

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
