/*
 * SQLiteLinux.h
 *
 *  Created on: 27.02.2016
 *      Author: steady286
 *      client database access for linux
 *		-
 */

#ifndef SQLITE3_CLIENTDATA_H_
#define SQLITE3_CLIENTDATA_H_
#include <stdint.h>
#include <Global_Defines.h>
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t last_insert_id;
} insert_node_return_t;

uint32_t sqlt3lnx_init();
void sqlt3lnx_close();

/* helpers .. todo: make these available for tests only */
void sqlt3lnx_deinit();


#ifdef __cplusplus
}
#endif

#endif /* CLIENTDATA_H_ */
