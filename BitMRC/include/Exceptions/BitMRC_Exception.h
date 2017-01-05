/*
 * BitMRC_Exception.h
 *
 *  Created on: 03.12.2016
 *      Author: blueboxes
 */

#ifndef BITMRC_EXCEPTION
#define BITMRC_EXCEPTION
#include <exception>

class BitMRC_Exception : public exception {
public:
	BitMRC_Exception();
	~BitMRC_Exception();
};

BitMRC_Exception::BitMRC_Exception () {
	cout << __func__ << endl;
}
BitMRC_Exception::~BitMRC_Exception () {
	cout << __func__ << endl;
}

#endif
