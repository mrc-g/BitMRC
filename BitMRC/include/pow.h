#pragma once
#ifndef _POW
#define _POW

#include <cryptlib.h>
#include <sha.h>
#include <hex.h>

#include <ctime>

#include <iostream>

#include <utils.h>
#include <bitmrc_endian.h> //need this only for getting the numbers swapped, since ustring made all the conversion automatically

//input:	data
//output:	first 32 byte of double sha512
ustring		doubleHash(ustring data);

//input:	nonce
//			payload
//output:	first 8 bytes of the pow
unsigned __int64		doPow(ustring data);

//input:	length
//			TTL
//			extrabytes	= 1000
//			noncetrials = 1000
//output:	target
__int64		getTarget(unsigned __int64 length, unsigned __int64 TTL, unsigned __int64 extrabytes = 1000, unsigned __int64 noncetrials = 1000);

//input		nonce
//			payload
//			extrabytes = 1000
//			noncetrials = 1000
//output:	check if is correct
bool		checkPow(ustring data, unsigned __int64 TTL, unsigned __int64 extrabytes = 1000, unsigned __int64 noncetrials = 1000);

//input:	payload
//output:	nonce found
__int64		searchPow(ustring data, unsigned __int64 TTL);

#endif
