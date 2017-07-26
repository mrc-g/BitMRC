/*
 * BitMRC_NoQueryConfigException.h
 *
 *  Created on: 03.12.2016
 *      Author: blueboxes
 */

#ifndef BITMRC_NOQUERYCONFIGEXCEPTION_H_
#define BITMRC_NOQUERYCONFIGEXCEPTION_H_

#include <Exceptions/BitMRC_Exception.h>

class BitMRC_NoQueryConfigException : public BitMRC_Exception {
public:
	BitMRC_NoQueryConfigException();
	~BitMRC_NoQueryConfigException();
};
BitMRC_NoQueryConfigException::BitMRC_NoQueryConfigException () {
	cout << __func__ << endl;
}
BitMRC_NoQueryConfigException::~BitMRC_NoQueryConfigException () {
	cout << __func__ << endl;
}
#endif /* BITMRC_NOQUERYCONFIGEXCEPTION_H_ */
