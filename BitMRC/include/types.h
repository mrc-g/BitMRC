/*
 * types.h
 * typemapping for BitMRC
 *  Created on: 25.03.2016
 *      Author: steady286
 */

#ifndef BITMRC_TYPES_H_
#define BITMRC_TYPES_H_
#ifdef LINUX
#include <stdint.h>
#include <unistd.h> /*sleep */

#define __int32 DONTUSEME
#define __uint32 DONTUSEME
#define __int16 DONTUSEME
#define __uint16 DONTUSEME
#define __uint64 DONTUSEME
#define __int64 DONTUSEME

	#define SOCK_TYPE int
	#define WSADATA int /* \todo: this is a hack just to compile NodeConnection*/
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define BYTE char
	#define DWORD unsigned long
	#define ZeroMemory(a,b) memset(a,0,b)
	#define WSAStartup(a,b)
#else

	#define SOCK_TYPE SOCKET
	#include <stdint.h>
	#define SLEEP Sleep
#endif

/* trace macros */
#define LOG_DB_TO_STDOUT  /*todo: move this to cmake cache variable (ccmake-configurable)*/

#ifdef LOG_DB_TO_STDOUT
#define LOG_DB(a) printf a
#else
#define LOG_DB(a)
#endif


/* function macros */
#ifdef LINUX
#define CLRMEM(m) memset(&m,0,sizeof(m));
#define closesocket close
#define SLEEP usleep
#else
#define CLRMEM(m) ZeroMemory( &hints, sizeof(hints) );
#endif



/* represent the system table, holding current settings for
 * the bitmrc
 */
typedef struct {
	uint64_t node_id;
	uint64_t working_mode; /* which features have been enabled ?*/
	uint64_t networking_flags; /* use ip4, use ip6 */
	uint16_t stream_ids[4]; /* we may process at most 4 streams */
	uint64_t last_startup_timestamp;
	uint32_t last_startup_result; /* keep track of last startup, so re-start loops can be avoided */
	uint16_t database_version;
} bitmrc_sysinfo_t;

typedef struct {
	uint32_t last_insert_id;
} insert_node_return_t;

typedef enum { BITMRC_OK,
		BITMRC_CLIENT_DB_INIT_BAD = 100 , BITMRC_DB_ACCESS_DENIED, BITMRC_DB_NOT_FOUND, BITMR_DB_OPEN_FAILED, BITMRC_DB_EXEC_FAILED, BITMRC_TABLE_CREATE_FAILED, /* db from 100 to 199 */
		BITMRC_SOME_ERROR = 200,
		BITMRC_BAD_PARA = 1000,
		BITMRC_ERR_MAX } enBitMRCError;

#endif /* TYPES_H_ */
