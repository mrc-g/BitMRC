/*
 * SQLiteLinux.c
 * implementation of sqlite3 database storage routines for BitMRC
 *
 *  Created on: 12.08.2016
 *      Author: steady286
 *      taken from another project :-)
 *
 *      note: the code is currently not re-entrant, and should be executed by
 *      one single thread only
 */
#include <stdint.h>
#include <Storage/SQLiteLinux.h>
#include <stdlib.h>
#include <sqlite3.h>

#define LOG_DB_TO_STDOUT  /*todo: move this to cmake cache variable (ccmake-configurable)*/

#ifdef LOG_DB_TO_STDOUT
#define LOG(a) printf a
#else
#define LOG(a)
#endif
/* when tests are enabled, expose internal functions */
#ifdef ENABLE_TESTS
#define PRIVATE
#else
#define PRIVATE static
#endif

enum { key_unused = 0, key_used = 1 } enKeyFlags;
enum { 	create_tables = 1, insert_node = 2,
		query_system = 3 , update_insert_delete = 4 } enSQL3QueryType;

static char init_ok;
static sqlite3 *db;
static unsigned char query[256];



static insert_node in;

static int is_sqlite_error(int errin) {
	if (errin == SQLITE_OK || errin == SQLITE_ROW || errin == SQLITE_DONE) {
		return 0;
	}
	return 1;
}
/** \brief generic sqlite3 exec function
 *
 */
PRIVATE uint32_t sqlt3lnx_sql_exec(uint16_t type, const unsigned char * query) {
	uint32_t ret = ERR_UNDEFINED;
	void * x_param = (void*)type;
	char * errinfo = NULL;
	int sret = sqlite3_exec(db, query, q_callback, x_param, &errinfo);
	if((is_sqlite_error(sret)) || errinfo != NULL ) {
		ret = ERR_DB_EXEC_FAILED;
		log_mgr_error( protmgr_get_type(), LOG_LVL_ERROR  ,"%s : sqlite3 exec %d error %s\n",__func__ , sret, errinfo);
		sqlite3_free(errinfo);
	} else {
		ret = ERR_OK;
	}
	return ret;
}
/** \brief this function gets called back by the sqlite3 engine after a query.
 * the parameter given to the query is returned here, so we can determine which query actually has been executed
 *
 */
PRIVATE int q_callback (void* param, int count, char** keys, char** values) {
    LOG_DB(("%s param %x columns: %d\n",__func__ , param, count))
	/* \todo: unify */
 	if ( (int)param == create_tables ) {
		LOG((" create tables\n"));
 	} else if ((int)param == query_system) {
 		/* system table has been queried */


	} else if ( (int)param == insert_node) {
		/* node storable has been stored , last_insert_id() returned as only field*/
		in.last_insert_id = (uint32_t)atoi(keys[0]);

		LOG((" inserted node id %u\n", in.last_insert_id)):
	return 0;
}


void sqlt3lnx_close() {
	if (db!=NULL)
		sqlite3_close(db);
}
void sqlt3lnx_deinit() {
	init_ok = 0 ;
}
/** \brief init the client data module
 * this also includes the sqlite-backend for the client
 * \return BITMRC_OK if everything went okay and db has been opened.
 * \return BITMRC_DB_NOT_FOUND  if db cannot be found
 * \return BITMRC_DB_ACCESS_DENIED if access was denied
 * \return BITMRC_CLIENT_DB_INIT_BAD if init failed
 * \return BITMR_DB_OPEN_FAILED if open failed
 */
uint32_t sqlt3lnx_init() {
	unsigned char db_path[] = {"bitmrc.sqlite3"};

	LOG(("%s : open db %s\n",__func__ , db_path));
		if ( sqlite3_initialize() != SQLITE_OK )
			return BITMRC_CLIENT_DB_INIT_BAD;
		if( sqlite3_open(db_path,&db) !=0 ) {
			return BITMR_DB_OPEN_FAILED;
		}
		LOG(("%s open db ok\n", __func__));
	}
	return BITMRC_OK;
}

/** \brief query the system table for settings
 * \return 0 if everything was okay
 * \return 1 if database table was not found
 * \return 2 if one or more columns were not found (todo)
 */
uint32_t sqlt3lnx_query_system_table(bitmrc_sysinfo_t * s_info) {
	uint32_t ret = ERR_BAD_PARA;
	char * errinfo = NULL;
	if (s_info == NULL) {
		return ret;
	}
	"" /* keep track of last startup, so re-start loops can be avoided */
	int sret = sqlite3_exec(db, "select node_id, working_mode, networking_flags,stream_id1, stream_id2, stream_id3, stream_id4, last_startup_timestamp,t last_startup_result from system;",
				q_callback,(void*)query_userstatus,&errinfo);
	if((is_sqlite_error(sret)) || errinfo != NULL ) {
		ret = ERR_DB_EXEC_FAILED;
		log_mgr_error( protmgr_get_type(), LOG_LVL_ERROR  ,"%s : sqlite3 exec %d error %s\n",__func__ , sret, errinfo);
		sqlite3_free(errinfo);

	} else {
		*u_info = user_status;
		ret = ERR_OK;
	}
	return ret;
}
 sqlt3lnx_TEMPL(def_mgmt_user_status_t * u_info) {
	uint32_t ret = ERR_OK;
	return ret;
}
