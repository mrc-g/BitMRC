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

#include <Storage/StorableQueryConfig.h>
#include <string.h>

StorableQueryConfig::StorableQueryConfig(uint16_t typeIn, char * queryIn, uint32_t max_rows_in, QueryResult * qrIn) :
	type(typeIn), bitmrc_error(0), qr(qrIn), last_id(0), max_rows(max_rows_in), returned_rows(0), query(NULL)
{
	if (queryIn != NULL && strlen(queryIn)>0) {
		query = new char [strlen(queryIn)+1];
		strcpy(query, queryIn);
	}
}
StorableQueryConfig::~StorableQueryConfig() {
	if(query!=NULL)
		delete [] query;
}
uint16_t StorableQueryConfig::getType() {
	return type;
}
uint32_t StorableQueryConfig::getBitMRCError() {
	return bitmrc_error;
}
char * StorableQueryConfig::getQuery() {
	return query;
}
QueryResult * StorableQueryConfig::getQueryResult() {
	return qr;
}
bool StorableQueryConfig::setBitMRCError(uint32_t errIn) {
	bitmrc_error = errIn;
	return true;
}
bool StorableQueryConfig::setQueryResult(QueryResult * qrIn) {
	qr = qrIn;
	return true;
}
bool StorableQueryConfig::setQuery(char * queryIn) {
	if(query != NULL)
		delete [] query;
	query = NULL;
	size_t ln = strlen(queryIn);
	if(queryIn != NULL) {
		query = new char [ln+1];
		if(query)
			strcpy(query, queryIn);
		return true;
	}
	return false;
}
void StorableQueryConfig::setMaxRows(uint32_t rows) {
	max_rows=rows;
}
uint32_t StorableQueryConfig::getLastId() {
	return last_id;
}
bool StorableQueryConfig::setRowSpec(RowSpec r) {
	rs = r;
	return true;
}
