/*
 * RowSpec.h
 *
 *  Created on: 04.12.2016
 *      Author: blueboxes
 */

#ifndef ROWSPEC_H_
#define ROWSPEC_H_

#include <stdint.h>
#include <Storage/ColumnSpec.h>
#include <vector>

#ifdef DEBUG_RSPEC
#define RS_DBG(a) printf a;
#else
#define RS_DBG(a)
#endif

class RowSpec {
public:
	RowSpec();
	~RowSpec();
public:
	bool addColumn(ColumnSpec cs);
	ColumnSpec * operator[](uint32_t index);
	uint32_t getColumnCount();
private:
	std::vector<ColumnSpec *> columns;
	uint32_t num_columns;

};

#endif /* ROWSPEC_H_ */
