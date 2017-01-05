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

uint64_t doPow(ustring data)
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

int64_t	getTarget(uint64_t length, uint64_t TTL, uint64_t extrabytes, uint64_t noncetrials)
{
	const uint64_t two_63 = UINT64_C(0x8000000000000000);
	uint64_t divisor;
	uint64_t target;
	if ((int32_t)TTL < 300 && (int32_t)TTL > -3600)
		TTL = 300;
	divisor = ((((length + (uint64_t)extrabytes) * TTL) / UINT64_C(0x10000)) + extrabytes + length) * noncetrials;

	/* We need to divide 2?? by divisor. We can't represent 2?? in
	* a 64-bit variable so instead we divide 2?� by the divisor
	* twice and add the result */
	target = two_63 / divisor * 2;
	/* If the fractional part of the result would be greater than
	* or equal to a half then we would get an extra 1 when we
	* multiply by two */
	if ((two_63 % divisor) * 2 >= divisor)
		target++;

	return target;
}

bool checkPow(ustring data, uint64_t TTL, uint64_t extrabytes, uint64_t noncetrial) {
	uint64_t pow = doPow(data);
	time_t ltime = std::time(nullptr);
	uint64_t target = getTarget(data.length() - 8, TTL - ltime);
	if (pow > target) {
		return false;
	} else {
		return true;
	}
}
uint64_t searchPow(ustring data, uint64_t TTL)
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

	uint64_t target = getTarget(payload.length(), TTL - ltime);
	uint64_t nonce = 0;

	char * tmpnonce = new char[8 + CryptoPP::SHA512::DIGESTSIZE];

	for (int i = 0; i < CryptoPP::SHA512::DIGESTSIZE; i++)
		tmpnonce[8 + i] = initial_hash[i];

	while (pow_value > target) {
		nonce++;

		tmpnonce[7] = nonce & 255;
		tmpnonce[6] = (nonce >> 8) & 255;
		tmpnonce[5] = (nonce >> 16) & 255;
		tmpnonce[4] = (nonce >> 24) & 255;
		tmpnonce[3] = (nonce >> 32) & 255;
		tmpnonce[2] = (nonce >> 40) & 255;
		tmpnonce[1] = (nonce >> 48) & 255;
		tmpnonce[0] = (nonce >> 56) & 255;

		hash.CalculateDigest(hash1, (byte*)tmpnonce, 8 + CryptoPP::SHA512::DIGESTSIZE);

		hash.CalculateDigest(hash2, hash1, sizeof(hash1));

		/* The POW value is the first 8 bytes of that
		* as a big-endian number */
		pow_value = BigLongLong(*(int64_t*)(hash2));
	}
	delete[] tmpnonce;
	return nonce;
}
