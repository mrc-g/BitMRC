
#ifndef _ENDIAN_H_
#define _ENDIAN_H_
#include <types.h>
/* use c linkage for non-c++ parts */

#ifdef __cplusplus
extern "C" {
#endif
extern short (*BigShort) ( short s );
extern short (*LittleShort) ( short s );
extern int (*BigLong) ( __int32 i );
extern int (*LittleLong) ( __int32 i );
extern __int64 (*BigLongLong) ( __int64 i );
extern __int64 (*LittleLongLong) ( __int64 i );
extern float (*BigFloat) ( float f );
extern float (*LittleFloat) ( float f );

extern void InitEndian( void );
extern bool BigEndianSystem;

short ShortSwap( short s );
short ShortNoSwap( short s );
int LongSwap (__int32 i);
int LongNoSwap( __int32 i );
__int64 LongLongSwap (__int64 i);
__int64 LongLongNoSwap( __int64 i );
float FloatSwap( float f );

#ifdef __cplusplus
}
#endif

#endif
