/*
 * StorableQueryConfig.h
 *
 *  Created on: 03.12.2016
 *      Author: blueboxes
 */

#ifndef STORABLEQUERYCONFIG_H_
#define STORABLEQUERYCONFIG_H_

#include <stdint.h>
#include <Storage/QueryResult.h>
#include <Storage/RowSpec.h>

typedef enum { SQC_NONE, SQC_INSERT, SQC_UPDATE, SQC_SELECT, SQC_CREATE, SQC_DROP, SQC_MAX } enStorableQueryConfigType;

class StorableQueryConfig {
public:
	StorableQueryConfig(uint16_t type, char * query, uint32_t max_rows, QueryResult * qr);
	~StorableQueryConfig();
public:
	uint16_t getType();
	uint32_t getBitMRCError();
	char * getQuery();
	QueryResult * getQueryResult();
	bool setBitMRCError(uint32_t errIn);
	bool setQueryResult(QueryResult * qrIn);
	bool setQuery(char *q);
	bool setRowSpec(RowSpec r);
	void setMaxRows(uint32_t rows);
	uint32_t getLastId();
private:
	uint16_t type;
	uint32_t last_id;
	uint32_t max_rows;
	uint32_t returned_rows;
	uint32_t bitmrc_error;
	QueryResult * qr;
	RowSpec rs;
	char * query;
};


#endif /* STORABLEQUERYCONFIG_H_ */
