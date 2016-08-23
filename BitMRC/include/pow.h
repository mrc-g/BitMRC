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
uint64_t doPow(ustring data);

//input:	length
//			TTL
//			extrabytes	= 1000
//			noncetrials = 1000
//output:	target
int64_t getTarget(uint64_t length, uint64_t TTL, uint64_t extrabytes = 1000, uint64_t noncetrials = 1000);

//input		nonce
//			payload
//			extrabytes = 1000
//			noncetrials = 1000
//output:	check if is correct
bool checkPow(ustring data, uint64_t TTL, uint64_t extrabytes = 1000, uint64_t noncetrials = 1000);

//input:	payload
//output:	nonce found
uint64_t searchPow(ustring data, uint64_t TTL);

#endif
