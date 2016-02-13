
#ifndef _ENDIAN
#define _ENDIAN


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

#endif