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
 * RowSpec.cpp
 *
 *  Created on: 04.12.2016
 *      Author: steady286
 */
#include <Storage/RowSpec.h>
#include <stdio.h>


RowSpec::RowSpec() {
	num_columns = 0;
	columns.clear();
}
RowSpec::~RowSpec() {

	RS_DBG(("%s deleting %u elements\n", __func__, num_columns));
	for (std::vector<ColumnSpec *>::iterator it = columns.begin() ; it != columns.end(); ++it) {
		RS_DBG(("%s deleting %s\n",__func__, (*it)->getName() ));
		delete *it;
	}
	columns.clear();
}

bool RowSpec::addColumn(ColumnSpec cs) {
	RS_DBG(("%s adding column %s\n", __func__, cs.getName()));
	//ColumnSpec * cs_tmp = new ColumnSpec(cs.getType(), cs.getName(), cs.getLen());
	uint16_t type = cs.getType();
	char * name = cs.getName();
	uint32_t len = cs.getLen();

	ColumnSpec * cs_tmp = new ColumnSpec(type, name, len);
	cs_tmp->print();
	columns.push_back(cs_tmp);
	num_columns++;
	return true;
}
ColumnSpec * RowSpec::operator[](uint32_t index) {
	if(index<num_columns) {
		return columns.operator [](index);
	} else {
		return NULL;
	}
}
uint32_t RowSpec::getColumnCount() {
	return num_columns;
}
