/*
 * ColumnSpec.h
 *
 *  Created on: 04.12.2016
 *      Author: steady286
 *      this class basically specifies one field in a table row
 *      it names the type, and the name. Used by class RowSpec
 */

#ifndef COLUMNSPEC_H_
#define COLUMNSPEC_H_

#ifdef DEBUG_CSPEC
#define CS_DBG(a) printf a;
#else
#define CS_DBG(a)
#endif

#include <stdint.h>

enum enColumnType { CS_TYPE_NONE = 0, CS_TYPE_INT16, CS_TYPE_INT32, CS_TYPE_INT64, CS_TYPE_UINT16,
	CS_TYPE_UINT32, CS_TYPE_UINT64, CS_TYPE_STRING, CS_TYPE_FLOAT, CS_TYPE_TEXT, CS_TYPE_BLOB, CS_TYPE_MAX };

class ColumnSpec {
public:
	ColumnSpec();
	ColumnSpec(uint16_t type, char * name, uint32_t len);
	~ColumnSpec();
public:
	uint16_t getType();
	char * getName();
	uint32_t getLen();
	void print();
private:
	uint16_t column_type;
	uint32_t column_len;
	char * column_name;
};



#endif /* COLUMNSPEC_H_ */
