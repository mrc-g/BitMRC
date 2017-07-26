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
 * ColumnSpec.cpp
 *
 *  Created on: 04.12.2016
 *      Author: steady286
 */
#include <Storage/ColumnSpec.h>
#include <stdio.h>
#include <string.h>


ColumnSpec::ColumnSpec() :
	column_type(CS_TYPE_NONE), column_len(0) {
}
ColumnSpec::ColumnSpec(uint16_t type, char * name, uint32_t len)
 : column_type(type), column_len(len), column_name(NULL) {
	if(type >= CS_TYPE_MAX) {
		column_type = CS_TYPE_NONE;
	}
	column_name = new char[(strlen(name)+1)];
	strcpy(column_name, name);
	CS_DBG(("%s column %s len %u type %hu\n", __func__, name, len, type));
}
ColumnSpec::~ColumnSpec() {
	if(column_name != NULL)
		delete [] column_name;
}
uint16_t ColumnSpec::getType() {
	return column_type;
}
char * ColumnSpec::getName() {
	return column_name;
}
uint32_t ColumnSpec::getLen() {
	return column_len;
}
void ColumnSpec::print() {
	CS_DBG(("   ColumnSpec: name %s, type %hd, len %u\n", column_name, column_type, column_len));
}
