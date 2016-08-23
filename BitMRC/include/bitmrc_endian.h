
#ifndef _ENDIAN_H_
#define _ENDIAN_H_
#include <types.h>
/* use c linkage for non-c++ parts */

#ifdef __cplusplus
extern "C" {
#endif
extern short (*BigShort) ( short s );
extern short (*LittleShort) ( short s );
extern int (*BigLong) (int32_t i);
extern int (*LittleLong) (int32_t i);
extern int64_t (*BigLongLong) (int64_t i);
extern int64_t (*LittleLongLong) (int64_t i);
extern float (*BigFloat) ( float f );
extern float (*LittleFloat) ( float f );

extern void InitEndian( void );
extern bool BigEndianSystem;

short ShortSwap( short s );
short ShortNoSwap( short s );
int LongSwap (int32_t i);
int LongNoSwap(int32_t i);
int64_t LongLongSwap (int64_t i);
int64_t LongLongNoSwap(int64_t i);
float FloatSwap( float f );

#ifdef __cplusplus
}
#endif

#endif
