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
 * QueryResultElement.cpp
 *
 *  Created on: 18.09.2016
 *      Author: steady286
 */
#include <Storage/QueryResultElement.h>
#include <string.h>
QueryResultElement::QueryResultElement() {
	i64.i = 0;
	i32.i = 0;
	data = NULL;
	string_len = 0;
	query_result = QR_RESULT_UNKNOWN;
	query_result_element_type = QRE_TYPE_NULL;
}
QueryResultElement::QueryResultElement(uint32_t value):QueryResultElement() {
	query_result_element_type = QRE_TYPE_UINT32;
	// QRE_DEBUG(( " val %u\n", value));
	i32.u = value;
}
QueryResultElement::QueryResultElement(double value):QueryResultElement() {
	query_result_element_type = QRE_TYPE_DOUBLE;
	// QRE_DEBUG(( " val %lf\n", value));
	floatval.dbl = value;
}
QueryResultElement::QueryResultElement(int32_t value) :QueryResultElement() {
	query_result_element_type = QRE_TYPE_INT32;
	// QRE_DEBUG(( " val %d\n", value));
	i32.i = value;
}
QueryResultElement::QueryResultElement(uint64_t value) :QueryResultElement() {
	query_result_element_type = QRE_TYPE_UINT64;
	// QRE_DEBUG(( " val %llu\n", value));
	i64.u = value;
}
QueryResultElement::QueryResultElement(int64_t value) :QueryResultElement() {
	query_result_element_type= QRE_TYPE_INT64;
	// QRE_DEBUG(( " val %lld\n", value));
	i64.i = value;
}
QueryResultElement::QueryResultElement(const char* nm, uint32_t len) :QueryResultElement() {
	query_result_element_type= QRE_TYPE_STRING;
	QRE_DEBUG(( " val '%s' len %u\n", nm, len));
	data = new char[len+1];
	if(data)
		strcpy(data, nm);
	string_len = len;
}
QueryResultElement::~QueryResultElement() {
	if(query_result_element_type == QRE_TYPE_STRING && string_len>0 && data != NULL) {
		delete [] data;
	}
}
uint32_t QueryResultElement::getUint32_value() {
	 return i32.u;
}
int32_t QueryResultElement::getInt32_value() {
	 return i32.i;
}
uint64_t QueryResultElement::getUint64_value() {
	 return i64.u;
}
int64_t QueryResultElement::getInt64_value() {
	 return i64.i;
}
char * QueryResultElement::getString_value() {
	return data;
}
char * QueryResultElement::getName() {
	return this->data;
}
bool QueryResultElement::setName(char * nameIn) {
	if(data != NULL)
		delete [] data;
	QRE_DEBUG(( " QRE %s : %s\n", __func__, nameIn));
	data = new char[strlen(nameIn)+1];
	if(data) {
		strcpy(data, nameIn);
		string_len = strlen(nameIn);
	} else {
		string_len = 0;
		return false;
	}
	return true;
}
uint16_t QueryResultElement::getType() {
	return query_result_element_type;
}
