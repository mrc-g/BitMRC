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
/**
 * \file QueryResult.cpp
 *
 *  Created on: 18.09.2016
 *      Author: steady286
 * \brief provde a base-class for all query - related results
 * this may be result-sets or just results, depending on the type of query
 *
 */
#include <Storage/QueryResult.h>
#include <string.h>
#include <types.h>

QueryResult::QueryResult(uint16_t type_in) : error_value(BITMRC_OK), native_error(0), error_string(NULL) {
	type = type_in;
	qrtv.clear();
}
/** \brief delete all result elements stored in the vector
 *
 */
QueryResult::~QueryResult() {
		QueryResultElement * qre = NULL;
		int sz = qrtv.size();
		while(sz>0) {
			qre = qrtv[--sz];
			delete qre;
		}
		qrtv.clear();
		if(error_string != NULL)
			delete [] error_string;
}
uint16_t QueryResult::getType() {
	return this->type;
}
bool QueryResult::addElement(QueryResultElement * qre) {

	qrtv.push_back(qre);
	// QR_DEBUG(("%s : add type %hu, '%s' count now %u	\n",__func__, qre->getType() , qre->getName(), qrtv.size()));
	return true;
}
bool QueryResult::getFieldUint32(char * name, uint32_t * out) {
	if (name == NULL || out == NULL)
		return false;

	int index = -1;
	if ((index = findField(name)) != -1) {
		*out = qrtv.operator [](index)->getUint32_value();
		return true;
	}
	return false;
}
bool QueryResult::getFieldInt32(char * name, int32_t * out) {
	if (name == NULL || out == NULL)
		return false;
	int index = -1;
	if ((index = findField(name)) != -1) {
		*out = qrtv.operator [](index)->getInt32_value();
		return true;
	}
	return false;
}
bool QueryResult::getFieldInt64(char * name, int64_t * out) {
	if (name == NULL || out == NULL)
		return false;
	int index = -1;
	if ((index = findField(name)) != -1) {
		*out = qrtv.operator [](index)->getInt64_value();
		return true;
	}
	return false;
}
bool QueryResult::getFieldUint64(char * name, uint64_t * out) {
	if (name == NULL || out == NULL)
		return false;
	int index = -1;
	if ((index = findField(name)) != -1) {
		*out = qrtv.operator [](index)->getUint64_value();
		return true;
	}
	return false;
}
bool QueryResult::getFieldString(char * name, char * out, uint32_t len) {
	if (name == NULL || out == NULL)
		return false;

	int index = -1;
	if ((index = findField(name)) != -1) {
		strncmp(out, qrtv.operator [](index)->getString_value(), len-1);
		return true;
	}
	return false;
}
uint32_t QueryResult::getNumColumns() {
	return qrtv.size();
}
/** \brief find a field by name,
 * \return its index
 * \return -1 if not found */
int32_t QueryResult::findField(char * name) {
	int32_t ret = -1;
	uint32_t cidx = 0;
	QR_DEBUG(("%s enter\n",__func__));
	for (std::vector<QueryResultElement*>::iterator it = qrtv.begin(); it != qrtv.end() ; it++) {
		QR_DEBUG(("%s : search field '%s'\n",__func__, (*it)->getName() ));
		if ((*it)->getName() != NULL && strcmp((*it)->getName(), name) == 0) {
			ret = cidx;
			QR_DEBUG(("%s : found field '%s'\n",__func__, (*it)->getName() ));
			break;
		}
		cidx++;
	}
	return ret;
}
void QueryResult::setError(uint16_t err) {
	this->error_value = err;
}
void QueryResult::setNativeError(int nerr) {
	this->native_error = nerr;
}
uint16_t QueryResult::getError() {
	return this->error_value;
}
int32_t QueryResult::getNativeError() {
	return this->native_error;
}
void  QueryResult::setErrorString(char * string_in) {
	if(error_string != NULL)
		delete [] error_string;

	error_string = new char[strlen(string_in)+1];
	if(error_string != NULL)
		strcpy(error_string, string_in);
}
char *  QueryResult::getErrorString() {
	return error_string;
}
