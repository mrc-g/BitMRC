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

__int64		searchPow(ustring data, unsigned __int64 TTL)
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

	while (pow_value > target)
	{
		nonce++;
		ustring nonceS;
		nonceS.appendInt64(nonce);
		hash.Update((byte*)nonceS.c_str(), nonceS.length());
		hash.Update(initial_hash, sizeof(initial_hash));
		hash.Final(hash1);


		hash.CalculateDigest(hash2, hash1, sizeof(hash1));

		/* The POW value is the last 8 bytes of that
		* as a big-endian number */
		memcpy(&pow_value, hash2, sizeof(pow_value));

		pow_value = BigLongLong(pow_value);
	}
	return nonce;
}