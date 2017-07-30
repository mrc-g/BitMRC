/*
 * QueryResult.h
 *
 *  Created on: 18.09.2016
 *      Author: steady286
 */

#ifndef QUERYRESULT_H_
#define QUERYRESULT_H_
#include <vector>
#include <stdint.h>
#include <Storage/QueryResult.h>
#include <Storage/QueryResultElement.h>
using namespace std;

#ifdef DEBUG_QR
#define QR_DEBUG(a) printf a;
#else
#define QR_DEBUG(a)
#endif

enum enQueryType { QR_TYPE_NONE = 0, QR_TYPE_UPDATE, QR_TYPE_SELECT, QR_TYPE_INSERT, QR_TYPE_CREATE, QR_TYPE_MAX};

class QueryResult {
public:
	QueryResult(uint16_t);
	~QueryResult();
public:
	uint16_t getType();
	bool addElement(QueryResultElement * qre);
	uint32_t getNumColumns();
public:
	void setError(uint16_t err);
	void setErrorString(char *);
	void setNativeError(int32_t nerr);
	uint16_t getError();
	int32_t getNativeError();
public:
	bool getFieldUint32(char * name, uint32_t * out);
	bool getFieldInt32(char * name, int32_t * out);
	bool getFieldInt64(char * name, int64_t * out);
	bool getFieldUint64(char * name, uint64_t * out);
	bool getFieldString(char * name, char * out, uint32_t len);
	char * getErrorString();
private:
	int32_t findField(char * name);
private:
	uint16_t type;
	uint16_t error_value;
	int32_t native_error;
	char * error_string;
	std::vector<QueryResultElement *> qrtv;
};
#endif /* QUERYRESULT_H_ */
