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
	// #error "is there a stdint header for win?"
	#define SLEEP Sleep
#endif
/* \todo: use stdint.h types only */
#define __int16 short
#define __uint16 unsigned short
#define __uint32 unsigned long
#define __int32 long
#define __int64 long long
#define __uint64 unsigned long long

/* function macros */
#ifdef LINUX
#define CLRMEM(m) memset(&m,0,sizeof(m));
#define closesocket close
#define SLEEP usleep
#else
#define CLRMEM(m) ZeroMemory( &hints, sizeof(hints) );
#endif



#endif /* TYPES_H_ */
