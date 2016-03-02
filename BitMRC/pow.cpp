#include "pow.h"

ustring		doubleHash(ustring data)
{
	ustring ret;
	CryptoPP::SHA512 hash;
	byte digest[CryptoPP::SHA512::DIGESTSIZE];
	byte digest2[CryptoPP::SHA512::DIGESTSIZE];
	hash.CalculateDigest(digest, (byte*)data.c_str(), data.length());
	hash.CalculateDigest(digest2, digest, sizeof digest);

	for (int i = 0; i < 32; i++)
	{
		ret.appendInt8((unsigned char)digest2[i]);
	}
	return ret;
}

unsigned __int64		doPow(ustring data)
{
	InitEndian();
	uint8_t initial_hash[CryptoPP::SHA512::DIGESTSIZE];
	uint8_t hash1[CryptoPP::SHA512::DIGESTSIZE];
	uint8_t hash2[CryptoPP::SHA512::DIGESTSIZE];
	uint64_t pow_value;

	CryptoPP::SHA512 hash;

	ustring payload;
	payload += data.substr(8, data.length());

	ustring nonce;
	nonce += data.substr(0, 8);

	hash.CalculateDigest(initial_hash, (byte*)payload.c_str(), payload.length());

	hash.Update((byte*)nonce.c_str(), nonce.length());
	hash.Update(initial_hash, sizeof(initial_hash));
	hash.Final(hash1);

	hash.CalculateDigest(hash2, hash1, sizeof(hash1));

	/* The POW value is the last 8 bytes of that
	* as a big-endian number */
	memcpy(&pow_value, hash2, sizeof(pow_value));

	pow_value = BigLongLong(pow_value);

	return pow_value;
}

__int64		getTarget(unsigned __int64 length, unsigned __int64 TTL, unsigned __int64 extrabytes, unsigned __int64 noncetrials)
{
	const uint64_t two_63 = UINT64_C(0x8000000000000000);
	uint64_t divisor;
	uint64_t target;
	if (TTL < 300)
		TTL = 300;
	divisor = ((((length + (uint64_t)extrabytes) * TTL) / UINT64_C(0x10000)) + extrabytes + length) * noncetrials;

	/* We need to divide 2?? by divisor. We can't represent 2?? in
	* a 64-bit variable so instead we divide 2?³ by the divisor
	* twice and add the result */
	target = two_63 / divisor * 2;
	/* If the fractional part of the result would be greater than
	* or equal to a half then we would get an extra 1 when we
	* multiply by two */
	if ((two_63 % divisor) * 2 >= divisor)
		target++;

	return target;
}

bool		checkPow(ustring data, unsigned __int64 TTL, unsigned __int64 extrabytes, unsigned __int64 noncetrial)
{
	unsigned __int64 pow = doPow(data);
	time_t ltime = std::time(nullptr);
	unsigned __int64 target = getTarget(data.length() - 8, TTL - ltime);
	if (pow > target)
	{
		return false;
	}
	else
	{
		return true;
	}
}


__int64		sP2(ustring data, unsigned __int64 TTL)
{
	InitEndian();
	uint8_t initial_hash[CryptoPP::SHA512::DIGESTSIZE];
	uint8_t hash1[CryptoPP::SHA512::DIGESTSIZE];
	uint8_t hash2[CryptoPP::SHA512::DIGESTSIZE];
	uint64_t pow_value = -1;

	CryptoPP::SHA512 hash;

	ustring payload;
	payload += data.substr(8, data.length());

	hash.CalculateDigest(initial_hash, (byte*)payload.c_str(), payload.length());

	time_t ltime = std::time(nullptr);

	unsigned __int64 target = getTarget(payload.length(), TTL - ltime);

	unsigned __int64 nonce = 0;

	char * tmpnonce = new char[8 + CryptoPP::SHA512::DIGESTSIZE];

	for (int i = 0; i < CryptoPP::SHA512::DIGESTSIZE; i++)
		tmpnonce[8 + i] = initial_hash[i];

	while (pow_value > target)
	{
		nonce++;


		unsigned char b1, b2, b3, b4, b5, b6, b7, b8;

		b1 = nonce & 255;
		b2 = (nonce >> 8) & 255;
		b3 = (nonce >> 16) & 255;
		b4 = (nonce >> 24) & 255;
		b5 = (nonce >> 32) & 255;
		b6 = (nonce >> 40) & 255;
		b7 = (nonce >> 48) & 255;
		b8 = (nonce >> 56) & 255;

		tmpnonce[0] = b8;
		tmpnonce[1] = b7;
		tmpnonce[2] = b6;
		tmpnonce[3] = b5;
		tmpnonce[4] = b4;
		tmpnonce[5] = b3;
		tmpnonce[6] = b2;
		tmpnonce[7] = b1;

		hash.CalculateDigest(hash1, (byte*)tmpnonce, 8 + CryptoPP::SHA512::DIGESTSIZE);

		hash.CalculateDigest(hash2, hash1, sizeof(hash1));

		/* The POW value is the first 8 bytes of that
		* as a big-endian number */
		pow_value = BigLongLong(*(__int64*)(hash2));
	}
	delete tmpnonce;
	return nonce;
}