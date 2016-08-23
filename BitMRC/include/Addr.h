//#pragma once
#ifndef _ADDR
#define _ADDR
#include "utils.h"
#include "base58.h"
#include "Packet.h"

class Addr;
class PubAddr;
class Unique_Key;

#include <cryptlib.h>
#include <dsa.h>
#include <sha.h>
#include <hex.h>

#include <Storage/Unique_Key.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>

using namespace std;
class PubAddr : public Storable {

public:
	PubAddr();
	PubAddr(const PubAddr &that);
	void operator=(const PubAddr &that);
	bool operator==(const PubAddr &that);
	//address with the BM-
	bool loadAddr(ustring address);
	//without prefix 0x04 length = 64
	//with prefix 0x04 length = 65
	//Skey signing key
	//Ekey encryption key
	bool loadKeys(ustring Skey, ustring Ekey, int nonce, int extra);
	bool decodeFromObj(packet_pubkey pubkey);
	//decode cipherText
	//pubk will agree with privk on the shared key that will be used for aes
	ustring decode(ustring data, ustring privK);
	//encode plain
	//return IV + CURVE (714 short) + public part of the privK (len X, X, len Y, Y) + cipherText + MAC
	ustring encode(ustring pubKA, ustring privKB, ustring pubKB, ustring plain);
	//get address BM-.....
	//need keys to start with 0x04
	ustring buildAddressFromKeys(ustring Skey, ustring Ekey, int stream, int version);
	//with prefix 0x04
	ustring getPubEncryptionKey();
	//with prefix 0x04
	ustring getPubSigningKey();
	//generate public key from private key
	ustring getPubOfPriv(ustring priv);
	int getNonce();
	int getExtra();
	int getVersion();
	int getStream();
	ustring getRipe();
	ustring getAddress();
	bool waitingPubKey();
	ustring getTag();
	ustring getTagE();
	time_t getLastPubKeyRequest();
	void setLastPubKeyRequest(time_t time);
public:
	/* storable base class methods */
	Unique_Key calc_key();
	bool query(Unique_Key &uq_key_in, string & data_out);
	bool store(Storable & object_in, Unique_Key & key_out);
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	Storable & 	find_by_key(Unique_Key &);

protected:
	ustring pubSigningKey;
	ustring pubEncryptionKey;

	//for async save of the key
	mutable std::shared_timed_mutex mutex_;

	//true if not known the keys
	//false if known
	bool empty;

	int extra_bytes;
	int nonce_trials;

	int version;
	int stream;
	ustring ripe;

	time_t lastPubKeyRequest;

	ustring address;
	ustring tag;
	ustring tagE;
};


class Addr : public PubAddr
{
public:
	Addr();

	Addr(const Addr &that);

	//load from keys and generate all the needed
	bool loadKeys(ustring pubE, ustring pubS, ustring privE, ustring privS, int stream, int version);

	packet_pubkey encodePubKey();

	//generate everything random
	bool generateRandom();

	//generate from a passphrase and a starting nonce just to know in what point of the deterministic generation we are.
	//return the new nonce.
	//if the new nonce is the same passed then failed.
	int generateDeterministic(ustring passphrase, int nonce);

	//with prefix 0x04
	ustring getPrivEncryptionKey();
	//with prefix 0x04
	ustring getPrivSigningKey();
	//with prefix 0x04
	ustring getPubEncryptionKey();
	//with prefix 0x04
	ustring getPubSigningKey();

private:
	ustring pubSigningKey;
	ustring privSigningKey;

	ustring pubEncryptionKey;
	ustring privEncryptionKey;

};


#endif
