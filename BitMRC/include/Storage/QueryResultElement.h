/*
* Copyright (c) 2016 steady286 github@dutka-is.de
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
/*
 * QueryResultElement.h
 *
 *  Created on: 18.09.2016
 *      Author: define one element inside the QueryResult
 */

#ifndef QUERYRESULTELEMENT_H_
#define QUERYRESULTELEMENT_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

#ifdef DEBUG_QRE
#define QRE_DEBUG(a) printf a;
#else
#define QRE_DEBUG(a)
#endif

enum enQueryResultType { QRE_TYPE_NONE = 0, QRE_TYPE_NULL, QRE_TYPE_INT32, QRE_TYPE_INT64, QRE_TYPE_UINT32, QRE_TYPE_UINT64, QRE_TYPE_STRING, QRE_TYPE_BLOB, QRE_TYPE_DOUBLE, QRE_TYPE_MAX };

enum enQueryResult { QR_RESULT_UNKNOWN, QR_RESULT_OK, QR_RESULT_ERROR };

class QueryResultElement {
public:
	QueryResultElement();
	QueryResultElement(uint32_t);
	QueryResultElement(int32_t);
	QueryResultElement(uint64_t);
	QueryResultElement(int64_t);
	QueryResultElement(const char*, uint32_t len);
	QueryResultElement(double value);
	~QueryResultElement();
	uint16_t getType();
	char * getName();
public:
	uint32_t getUint32_value();
	int32_t getInt32_value();
	uint64_t getUint64_value();
	int64_t getInt64_value();
	char * getString_value();
	bool setName(char * nameIn);
private:
	uint32_t string_len;
	uint16_t query_result_element_type;
	uint16_t query_result;
	uint16_t num_elements;
	union int64 {
		uint64_t u;
		int64_t i;
	} i64;
	union uint32 {
		uint32_t u;
		int32_t i;
	} i32;
	union {
		double dbl;
		float flt;
	} floatval;
	char * data;



};


#endif /* QUERYRESULTELEMENT_H_ */
