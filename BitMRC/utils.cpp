#include "utils.h"

string ustring::toString()
{
	string ret;
	for(unsigned int i=0;i<this->length();i++)
		ret += this->operator[](i);
	return ret;
}

void ustring::fromString(string from)
{
	this->append((unsigned char*)from.c_str(),from.size());
}

void ustring::appendInt8(unsigned char i)
{
	this->operator+=(i);
}

void ustring::appendInt16(__int16 i)
{

	unsigned char b1, b2;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;

	this->appendInt8(b2);
	this->appendInt8(b1);
}

void ustring::appendInt32(__int32 i)
{

	unsigned char b1, b2, b3, b4;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i >>16 ) & 255;
	b4 = ( i >>24 ) & 255;
		
	this->appendInt8(b4);
	this->appendInt8(b3);
	this->appendInt8(b2);
	this->appendInt8(b1);

}

void ustring::appendInt64(__int64 i)
{
	unsigned char b1, b2, b3, b4, b5, b6, b7, b8;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i >> 16 ) & 255;
	b4 = ( i >> 24 ) & 255;
	b5 = ( i >> 32 ) & 255;
	b6 = ( i >> 40 ) & 255;
	b7 = ( i >> 48 ) & 255;
	b8 = ( i >> 56 ) & 255;
	
	this->appendInt8(b8);
	this->appendInt8(b7);
	this->appendInt8(b6);
	this->appendInt8(b5);
	this->appendInt8(b4);
	this->appendInt8(b3);
	this->appendInt8(b2);
	this->appendInt8(b1);	
}

void ustring::appendVarInt_B(unsigned __int64 i)
{
    if(i < 0)
        return; // FIXME: throw exception or something
   
    if (i < 253)
	{
		this->appendInt8( (unsigned char) i );
        return;
	}
	else if ((i >= 253) && (i < 65536)) 
	{
        this->appendInt8( (unsigned char) 253 );
		this->appendInt16( (unsigned short) i );
		return;
	}
    else if ((i >= 65536) && (i < 4294967296))
	{
        this->appendInt8( (unsigned char) 254 );
		this->appendInt32( (unsigned __int32)(i));
		return;
	}
    else if (i >= 4294967296)
	{
        this->appendInt8( (unsigned char) 255 );
		this->appendInt64( (unsigned __int64) i );
		return;
	}
	return;
}

void ustring::appendVarInt32(unsigned __int32 i)
{
	while (i >= 0x80) {
		this->appendInt8(static_cast<unsigned char>( i | 0x80 ));
		i >>= 7;
	}
	this->appendInt8( static_cast<unsigned char>(i) );
	return;
}

void ustring::appendVarInt64(unsigned __int64 i)
{
	// Splitting into 32-bit pieces gives better performance on 32-bit
	// processors.
	unsigned __int32 part0 = static_cast<unsigned __int32>( i );
	unsigned __int32 part1 = static_cast<unsigned __int32>( i >> 28 );
	unsigned __int32 part2 = static_cast<unsigned __int32>( i >> 56 );

	int size;

	// Here we can't really optimize for small numbers, since the value is
	// split into three parts.  Cheking for numbers < 128, for instance,
	// would require three comparisons, since you'd have to make sure part1
	// and part2 are zero.  However, if the caller is using 64-bit integers,
	// it is likely that they expect the numbers to often be very large, so
	// we probably don't want to optimize for small numbers anyway.  Thus,
	// we end up with a hardcoded binary search tree...
	if (part2 == 0) {
		if (part1 == 0) {
			if (part0 < (1 << 14)) {
				if (part0 < (1 << 7)) {
					size = 1;
				} else {
					size = 2;
				}
			} else {
				if (part0 < (1 << 21)) {
					size = 3;
				} else {
					size = 4;
				}
			}
		} else {
			if (part1 < (1 << 14)) {
				if (part1 < (1 << 7)) {
					size = 5;
				} else {
					size = 6;
				}
			} else {
				if (part1 < (1 << 21)) {
					size = 7;
				} else {
					size = 8;
				}
			}
		}
	} else {
		if (part2 < (1 << 7)) {
			size = 9;
		} else {
			size = 10;
		}
	}

	for(int j=0; j<size; j++)
	{
		this->appendInt8( static_cast<unsigned char>((part0  >> j*7   ) | 0x80));
	}
	for(int j=0; j<size-4; j++)
	{
		this->appendInt8( static_cast<unsigned char>((part0  >> j*7   ) | 0x80));
	}
	for(int j=0; j<size-8; j++)
	{
		this->appendInt8( static_cast<unsigned char>((part0  >> j*7   ) | 0x80));
	}

	unsigned char tmp= this->operator[](this->length()-1);
	this->operator[](this->length()-1) = tmp & 0x7F;
	return;
}


unsigned char ustring::getInt8(unsigned int& i)
{
	if (this->size() <= i)
		throw runtime_error("ustring out of range");
	return this->operator[](i++);
}

__int16 ustring::getInt16(unsigned int& i)
{
	__int16 ret;
	ret = (__int16)(((unsigned __int16)this->getInt8(i) << 8) + (unsigned __int16)this->getInt8(i));
	return ret;
}

__int32 ustring::getInt32(unsigned int& i)
{
	__int32 ret;
	ret = (__int32)(((unsigned __int32)this->getInt8(i) << 24) + ((unsigned __int32)this->getInt8(i) << 16) + ((unsigned __int32)this->getInt8(i) << 8) + (unsigned __int32)this->getInt8(i));
	return ret;
}

__int64 ustring::getInt64(unsigned int& i)
{
	__int64 ret;
	ret = (__int64)(((unsigned __int64)this->getInt8(i) << 56) + ((unsigned __int64)this->getInt8(i) << 48) + ((unsigned __int64)this->getInt8(i) << 40) + ((unsigned __int64)this->getInt8(i) << 32) + ((unsigned __int64)this->getInt8(i) << 24) + ((unsigned __int64)this->getInt8(i) << 16) + ((unsigned __int64)this->getInt8(i) << 8) + (unsigned __int64)this->getInt8(i));
	return ret;
}

__int64 ustring::getVarInt_B(unsigned int& i)
{
	unsigned char firstByte;

	if (this->length() <= i) {
        return 0;
	}

    firstByte = (unsigned char)this->getInt8(i);

    if (firstByte < 253) {
        return firstByte;
	}
    if (firstByte == 253) {
		return ((unsigned short)this->getInt8(i) << 8) + (unsigned short)this->getInt8(i);
	}
    if (firstByte == 254) {
		return ((unsigned __int32)this->getInt8(i) << 24) + ((unsigned __int32)this->getInt8(i) << 16) + ((unsigned __int32)this->getInt8(i) << 8) + (unsigned __int32)this->getInt8(i);
	}
    if (firstByte == 255) {
        return ((unsigned __int64)this->getInt8(i) << 56) + ((unsigned __int64)this->getInt8(i) << 48) + ((unsigned __int64)this->getInt8(i) << 40) + ((unsigned __int64)this->getInt8(i) << 32) + ((unsigned __int64)this->getInt8(i) << 24) + ((unsigned __int64)this->getInt8(i) << 16) + ((unsigned __int64)this->getInt8(i) << 8) + (unsigned __int64)this->getInt8(i);
	}
	return 0;
}

__int32 ustring::getVarInt32(unsigned int& i)
{
	__int32 b;
	__int32 result = 0;

	for(unsigned int j=0; (j < 5 ) && (i < this->length()); j++)
	{
		b = (unsigned __int32)this->getInt8(i);	result += b << 7*j;	if (!(b & 0x80)) goto done;
		result -= 0x80;
	}

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits. max - max32 = 10 - 5 = 5
	for (unsigned int j = 0; (j < 5) && (j < this->length()); j++) {
		b = (unsigned __int32)this->getInt8(i); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	i = -1;
	return NULL;
done:

	return result;
}

__int64 ustring::getVarInt64(unsigned int& i)
{
	__int64 b;
	__int64 result = 0;

	for(int j=0; (j < 5 ) && (i < this->length()); j++)
	{
		b = (unsigned __int64)this->getInt8(i);	result += b << 7*j;	if (!(b & 0x80)) goto done;
		result -= 0x80;
	}

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits. max - max32 = 10 - 5 = 5
	for (unsigned int j = 0; (j < 5) && (j < this->length()); j++) {
		b = (unsigned __int64)this->getInt8(i); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	i = -1;
	return NULL;
done:

	return result;
}

void ustring::appendVarString_B(string str)
{
	this->appendVarInt_B(str.length());
	ustring tmp;
	tmp.fromString(str);
	this->operator+=(tmp);
}

void ustring::appendVarUstring_B(ustring str)
{
	this->appendVarInt_B(str.length());
	this->operator+=(str);
}

void ustring::appendVarString(string str)
{
	this->appendVarInt32(str.length());
	ustring tmp;
	tmp.fromString(str);
	this->operator+=(tmp);
}

void ustring::appendVarUstring(ustring str)
{
	this->appendVarInt32(str.length());
	this->operator+=(str);
}

string ustring::getString(int len, unsigned int& i)
{
	ustring tmp;
	tmp += this->substr(i,len);
	string ret;
	ret = tmp.toString();
	i +=len;
	return ret;
}

ustring ustring::getUstring(int len, unsigned int& i)
{
	ustring ret;
	ret += this->substr(i,len);
	i +=len;
	return ret;
}

string ustring::getVarString(unsigned int& i)
{
	int len = this->getVarInt32(i);
	string ret;
	ustring tmp;
	tmp += this->substr(i,len);
	ret = tmp.toString();
	i +=len;
	return ret;
}

ustring ustring::getVarUstring(unsigned int& i)
{
	int len = this->getVarInt32(i);
	ustring ret;
	ret += this->substr(i,len);
	i +=len;
	return ret;
}

string ustring::getVarString_B(unsigned int& i)
{
	int len = (int)this->getVarInt_B(i);
	string ret;
	ustring tmp;
	tmp += this->substr(i,len);
	ret = tmp.toString();
	i +=len;
	return ret;
}

ustring ustring::getVarUstring_B(unsigned int& i)
{
	ustring ret;
	int len = (int)this->getVarInt_B(i);
	ret += this->substr(i,len);
	i +=len;
	return ret;
}

ustring ustring::getRest(unsigned int& i)
{
	ustring ret;
	ret += this->substr(i, this->length()-i);
	i = this->length();
	return ret;
}

/*ustring ustring::operator+(ustring adder)
{
	ustring ret;

	for (int i = 0; i<adder.length(); i++)
		this->operator+=(adder[i]);

	return ret;
}*/

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
//methods of socket_ustring
void socket_ustring::sendByte(unsigned char i)
{
	char byte[1];
	byte[0]=i;
	int iResult = send( this->socket, byte, 1, 0 );
    if (iResult == SOCKET_ERROR) {
		throw CONNECTION_ERROR;
    }
}

void socket_ustring::sendInt16(__int16 i)
{

	unsigned char b1, b2;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;

	this->sendByte(b2);
	this->sendByte(b1);
}

void socket_ustring::sendInt32(__int32 i)
{

	unsigned char b1, b2, b3, b4;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i >>16 ) & 255;
	b4 = ( i >>24 ) & 255;
		
	this->sendByte(b4);
	this->sendByte(b3);
	this->sendByte(b2);
	this->sendByte(b1);

}

void socket_ustring::sendInt64(__int64 i)
{
	unsigned char b1, b2, b3, b4, b5, b6, b7, b8;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i >> 16 ) & 255;
	b4 = ( i >> 24 ) & 255;
	b5 = ( i >> 32 ) & 255;
	b6 = ( i >> 40 ) & 255;
	b7 = ( i >> 48 ) & 255;
	b8 = ( i >> 56 ) & 255;
	
	this->sendByte(b8);
	this->sendByte(b7);
	this->sendByte(b6);
	this->sendByte(b5);
	this->sendByte(b4);
	this->sendByte(b3);
	this->sendByte(b2);
	this->sendByte(b1);	
}

void socket_ustring::sendVarInt_B(unsigned __int64 i)
{
    if(i < 0)
        return; // FIXME: throw exception or something
   
    if (i < 253)
	{
		this->sendByte( (unsigned char) i );
        return;
	}
	else if ((i >= 253) && (i < 65536)) 
	{
        this->sendByte( (unsigned char) 253 );
		this->sendInt16( (unsigned short) i );
		return;
	}
    else if ((i >= 65536) && (i < 4294967296))
	{
        this->sendByte( (unsigned char) 254 );
		this->sendInt32( (unsigned __int32)(i));
		return;
	}
    else if (i >= 4294967296)
	{
        this->sendByte( (unsigned char) 255 );
		this->sendInt64( (unsigned __int64) i );
		return;
	}
	return;
}

void socket_ustring::sendVarInt32(unsigned __int32 i)
{
	while (i >= 0x80) {
		this->sendByte(static_cast<unsigned char>( i | 0x80 ));
		i >>= 7;
	}
	this->sendByte( static_cast<unsigned char>(i) );
	return;
}

void socket_ustring::sendVarInt64(unsigned __int64 i)
{
	// Splitting into 32-bit pieces gives better performance on 32-bit
	// processors.
	unsigned __int32 part0 = static_cast<unsigned __int32>( i );
	unsigned __int32 part1 = static_cast<unsigned __int32>( i >> 28 );
	unsigned __int32 part2 = static_cast<unsigned __int32>( i >> 56 );

	int size;

	// Here we can't really optimize for small numbers, since the value is
	// split into three parts.  Cheking for numbers < 128, for instance,
	// would require three comparisons, since you'd have to make sure part1
	// and part2 are zero.  However, if the caller is using 64-bit integers,
	// it is likely that they expect the numbers to often be very large, so
	// we probably don't want to optimize for small numbers anyway.  Thus,
	// we end up with a hardcoded binary search tree...
	if (part2 == 0) {
		if (part1 == 0) {
			if (part0 < (1 << 14)) {
				if (part0 < (1 << 7)) {
					size = 1;
				} else {
					size = 2;
				}
			} else {
				if (part0 < (1 << 21)) {
					size = 3;
				} else {
					size = 4;
				}
			}
		} else {
			if (part1 < (1 << 14)) {
				if (part1 < (1 << 7)) {
					size = 5;
				} else {
					size = 6;
				}
			} else {
				if (part1 < (1 << 21)) {
					size = 7;
				} else {
					size = 8;
				}
			}
		}
	} else {
		if (part2 < (1 << 7)) {
			size = 9;
		} else {
			size = 10;
		}
	}

	ustring tmp_s;

	for(int j=0; j<size; j++)
	{
		tmp_s.appendInt8( static_cast<unsigned char>((part0  >> j*7   ) | 0x80));
	}
	for(int j=0; j<size-4; j++)
	{
		tmp_s.appendInt8( static_cast<unsigned char>((part0  >> j*7   ) | 0x80));
	}
	for(int j=0; j<size-8; j++)
	{
		tmp_s.appendInt8( static_cast<unsigned char>((part0  >> j*7   ) | 0x80));
	}

	

	unsigned char tmp= tmp_s[tmp_s.length()-1];
	tmp_s[tmp_s.length()-1] = tmp & 0x7F;

	this->sendUstring(tmp_s);

	return;
}

void socket_ustring::sendString(string str, int len)
{
	for(unsigned int i=0; (int)i<len;i++)
	{
		if(i>= str.length())
		{
			this->sendByte(0);
			continue;
		}
		this->sendByte(str[i]);
	}
}
	
void socket_ustring::sendUstring(ustring str, int len)
{
	for(unsigned int i=0; (int)i<len;i++)
	{
		if(i>= str.length())
		{
			this->sendByte(0);
			continue;
		}
		this->sendByte(str[i]);
	}
}

void socket_ustring::sendString(string str)
{
	for(unsigned int i=0; i<str.length();i++)
	{
		this->sendByte(str[i]);
	}
}
	
void socket_ustring::sendUstring(ustring str)
{
	for(unsigned int i=0; i<str.length();i++)
	{
		this->sendByte(str[i]);
	}
}

void socket_ustring::sendVarString_B(string str)
{
	this->sendVarInt_B(str.length());
	this->sendString(str);
}

void socket_ustring::sendVarUstring_B(ustring str)
{
	this->sendVarInt_B(str.length());
	this->sendUstring(str);
}

void socket_ustring::sendVarString(string str)
{
	this->sendVarInt32(str.length());
	this->sendString(str);
}

void socket_ustring::sendVarUstring(ustring str)
{
	this->sendVarInt64(str.length());
	this->sendUstring(str);
}

unsigned char socket_ustring::getInt8()
{
	char byte[1];

	int iResult = recv(this->socket, byte, 1, 0);
    if ( iResult > 0 )
		return (unsigned char)byte[0];
    else if ( iResult == 0 )
		throw CONNECTION_CLOSED;
    else
	{
		//printf("recv failed with error: %d\n", WSAGetLastError());
		throw CONNECTION_ERROR;
	}
}

__int16 socket_ustring::getInt16()
{
	__int16 ret;
	ret = (__int16)(((unsigned __int16)this->getInt8() << 8) + (unsigned __int16)this->getInt8());
	return ret;
}

__int32 socket_ustring::getInt32()
{
	__int32 ret;
	ret = (__int32)(((unsigned __int32)this->getInt8() << 24) + ((unsigned __int32)this->getInt8() << 16) + ((unsigned __int32)this->getInt8() << 8) + (unsigned __int32)this->getInt8());
	return ret;
}

__int64 socket_ustring::getInt64()
{
	__int64 ret;
	ret = (__int64)(((unsigned __int64)this->getInt8() << 56) + ((unsigned __int64)this->getInt8() << 48) + ((unsigned __int64)this->getInt8() << 40) + ((unsigned __int64)this->getInt8() << 32) + ((unsigned __int64)this->getInt8() << 24) + ((unsigned __int64)this->getInt8() << 16) + ((unsigned __int64)this->getInt8() << 8) + (unsigned __int64)this->getInt8());
	return ret;
}

__int64 socket_ustring::getVarInt_B()
{
	unsigned char firstByte;

    firstByte = (unsigned char)this->getInt8();

    if (firstByte < 253) {
        return firstByte;
	}
    if (firstByte == 253) {
		return ((unsigned short)this->getInt8() << 8) + (unsigned short)this->getInt8();
	}
    if (firstByte == 254) {
		return ((unsigned __int32)this->getInt8() << 24) + ((unsigned __int32)this->getInt8() << 16) + ((unsigned __int32)this->getInt8() << 8) + (unsigned __int32)this->getInt8();
	}
    if (firstByte == 255) {
        return ((unsigned __int64)this->getInt8() << 56) + ((unsigned __int64)this->getInt8() << 48) + ((unsigned __int64)this->getInt8() << 40) + ((unsigned __int64)this->getInt8() << 32) + ((unsigned __int64)this->getInt8() << 24) + ((unsigned __int64)this->getInt8() << 16) + ((unsigned __int64)this->getInt8() << 8) + (unsigned __int64)this->getInt8();
	}
	return 0;
}

__int32 socket_ustring::getVarInt32()
{
	__int32 b;
	__int32 result = 0;

	for(int j=0; j < 5; j++)
	{
		b = (unsigned __int32)this->getInt8();	result += b << 7*j;	if (!(b & 0x80)) goto done;
		result -= 0x80;
	}

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits. max - max32 = 10 - 5 = 5
	for (int j = 0; j < 5; j++) {
		b = (unsigned __int32)this->getInt8(); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	return NULL;
done:

	return result;
}

__int64 socket_ustring::getVarInt64()
{
	__int64 b;
	__int64 result = 0;

	for(int j=0; j < 5 ; j++)
	{
		b = (unsigned __int64)this->getInt8();	result += b << 7*j;	if (!(b & 0x80)) goto done;
		result -= 0x80;
	}

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits. max - max32 = 10 - 5 = 5
	for (int j = 0; j < 5 ; j++) {
		b = (unsigned __int64)this->getInt8(); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	return NULL;
done:

	return result;
}

string socket_ustring::getString(int len)
{
	string ret;
	for(int i=0;i<len;i++)
	{
		BYTE tmp = this->getInt8();
		ret += tmp;
	}
	if (ret.empty())
		return string();
	return ret;
}

ustring socket_ustring::getUstring(int len)
{
	ustring ret;
	for(int i=0;i<len;i++)
	{
		BYTE tmp = this->getInt8();
		ret += tmp;
	}
	if (ret.empty())
		return ustring();
	return ret;
}

string socket_ustring::getVarString()
{
	int len = this->getVarInt32();
	string ret = this->getString(len);
	return ret;
}

ustring socket_ustring::getVarUstring()
{
	int len = this->getVarInt32();
	ustring ret = this->getUstring(len);
	return ret;
}

string socket_ustring::getVarString_B()
{
	int len = (int)this->getVarInt_B();
	string ret = this->getString(len);
	return ret;
}

ustring socket_ustring::getVarUstring_B()
{
	int len = (int)this->getVarInt_B();
	ustring ret = this->getUstring(len);
	return ret;
}

void socket_ustring::setSocket(SOCKET sock)
{
	this->socket = sock;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
//methods of file_ustring

void file_ustring::writeByte(unsigned char i)
{
	char byte[1];
	byte[0] = i;
	int iResult = fwrite(byte, 1, 1, this->pFile);
	if (iResult != 1) {
		throw 1;
	}
}

void file_ustring::writeInt16(__int16 i)
{

	unsigned char b1, b2;

	b1 = i & 255;
	b2 = (i >> 8) & 255;

	this->writeByte(b2);
	this->writeByte(b1);
}

void file_ustring::writeInt32(__int32 i)
{

	unsigned char b1, b2, b3, b4;

	b1 = i & 255;
	b2 = (i >> 8) & 255;
	b3 = (i >> 16) & 255;
	b4 = (i >> 24) & 255;

	this->writeByte(b4);
	this->writeByte(b3);
	this->writeByte(b2);
	this->writeByte(b1);

}

void file_ustring::writeInt64(__int64 i)
{
	unsigned char b1, b2, b3, b4, b5, b6, b7, b8;

	b1 = i & 255;
	b2 = (i >> 8) & 255;
	b3 = (i >> 16) & 255;
	b4 = (i >> 24) & 255;
	b5 = (i >> 32) & 255;
	b6 = (i >> 40) & 255;
	b7 = (i >> 48) & 255;
	b8 = (i >> 56) & 255;

	this->writeByte(b8);
	this->writeByte(b7);
	this->writeByte(b6);
	this->writeByte(b5);
	this->writeByte(b4);
	this->writeByte(b3);
	this->writeByte(b2);
	this->writeByte(b1);
}

void file_ustring::writeVarInt_B(unsigned __int64 i)
{
	if (i < 0)
		return; // FIXME: throw exception or something

	if (i < 253)
	{
		this->writeByte((unsigned char)i);
		return;
	}
	else if ((i >= 253) && (i < 65536))
	{
		this->writeByte((unsigned char)253);
		this->writeInt16((unsigned short)i);
		return;
	}
	else if ((i >= 65536) && (i < 4294967296))
	{
		this->writeByte((unsigned char)254);
		this->writeInt32((unsigned __int32)(i));
		return;
	}
	else if (i >= 4294967296)
	{
		this->writeByte((unsigned char)255);
		this->writeInt64((unsigned __int64)i);
		return;
	}
	return;
}

void file_ustring::writeVarInt32(unsigned __int32 i)
{
	while (i >= 0x80) {
		this->writeByte(static_cast<unsigned char>(i | 0x80));
		i >>= 7;
	}
	this->writeByte(static_cast<unsigned char>(i));
	return;
}

void file_ustring::writeVarInt64(unsigned __int64 i)
{
	// Splitting into 32-bit pieces gives better performance on 32-bit
	// processors.
	unsigned __int32 part0 = static_cast<unsigned __int32>(i);
	unsigned __int32 part1 = static_cast<unsigned __int32>(i >> 28);
	unsigned __int32 part2 = static_cast<unsigned __int32>(i >> 56);

	int size;

	// Here we can't really optimize for small numbers, since the value is
	// split into three parts.  Cheking for numbers < 128, for instance,
	// would require three comparisons, since you'd have to make sure part1
	// and part2 are zero.  However, if the caller is using 64-bit integers,
	// it is likely that they expect the numbers to often be very large, so
	// we probably don't want to optimize for small numbers anyway.  Thus,
	// we end up with a hardcoded binary search tree...
	if (part2 == 0) {
		if (part1 == 0) {
			if (part0 < (1 << 14)) {
				if (part0 < (1 << 7)) {
					size = 1;
				}
				else {
					size = 2;
				}
			}
			else {
				if (part0 < (1 << 21)) {
					size = 3;
				}
				else {
					size = 4;
				}
			}
		}
		else {
			if (part1 < (1 << 14)) {
				if (part1 < (1 << 7)) {
					size = 5;
				}
				else {
					size = 6;
				}
			}
			else {
				if (part1 < (1 << 21)) {
					size = 7;
				}
				else {
					size = 8;
				}
			}
		}
	}
	else {
		if (part2 < (1 << 7)) {
			size = 9;
		}
		else {
			size = 10;
		}
	}

	ustring tmp_s;

	for (int j = 0; j<size; j++)
	{
		tmp_s.appendInt8(static_cast<unsigned char>((part0 >> j * 7) | 0x80));
	}
	for (int j = 0; j<size - 4; j++)
	{
		tmp_s.appendInt8(static_cast<unsigned char>((part0 >> j * 7) | 0x80));
	}
	for (int j = 0; j<size - 8; j++)
	{
		tmp_s.appendInt8(static_cast<unsigned char>((part0 >> j * 7) | 0x80));
	}



	unsigned char tmp = tmp_s[tmp_s.length() - 1];
	tmp_s[tmp_s.length() - 1] = tmp & 0x7F;

	this->writeUstring(tmp_s);

	return;
}

void file_ustring::writeString(string str, int len)
{
	for (unsigned int i = 0; (int)i<len; i++)
	{
		if (i >= str.length())
		{
			this->writeByte(0);
			continue;
		}
		this->writeByte(str[i]);
	}
}

void file_ustring::writeUstring(ustring str, int len)
{
	for (unsigned int i = 0; (int)i<len; i++)
	{
		if (i >= str.length())
		{
			this->writeByte(0);
			continue;
		}
		this->writeByte(str[i]);
	}
}

void file_ustring::writeString(string str)
{
	for (unsigned int i = 0; i<str.length(); i++)
	{
		this->writeByte(str[i]);
	}
}

void file_ustring::writeUstring(ustring str)
{
	for (unsigned int i = 0; i<str.length(); i++)
	{
		this->writeByte(str[i]);
	}
}

void file_ustring::writeVarString_B(string str)
{
	this->writeVarInt_B(str.length());
	this->writeString(str);
}

void file_ustring::writeVarUstring_B(ustring str)
{
	this->writeVarInt_B(str.length());
	this->writeUstring(str);
}

void file_ustring::writeVarString(string str)
{
	this->writeVarInt32(str.length());
	this->writeString(str);
}

void file_ustring::writeVarUstring(ustring str)
{
	this->writeVarInt64(str.length());
	this->writeUstring(str);
}

unsigned char file_ustring::getInt8()
{
	char byte[1];

	int iResult = fread(byte, 1, 1, this->pFile);
	if (iResult == 0)
		throw 1;
	return (unsigned char)byte[0];
}

__int16 file_ustring::getInt16()
{
	__int16 ret;
	ret = (__int16)(((unsigned __int16)this->getInt8() << 8) + (unsigned __int16)this->getInt8());
	return ret;
}

__int32 file_ustring::getInt32()
{
	__int32 ret;
	ret = (__int32)(((unsigned __int32)this->getInt8() << 24) + ((unsigned __int32)this->getInt8() << 16) + ((unsigned __int32)this->getInt8() << 8) + (unsigned __int32)this->getInt8());
	return ret;
}

__int64 file_ustring::getInt64()
{
	__int64 ret;
	ret = (__int64)(((unsigned __int64)this->getInt8() << 56) + ((unsigned __int64)this->getInt8() << 48) + ((unsigned __int64)this->getInt8() << 40) + ((unsigned __int64)this->getInt8() << 32) + ((unsigned __int64)this->getInt8() << 24) + ((unsigned __int64)this->getInt8() << 16) + ((unsigned __int64)this->getInt8() << 8) + (unsigned __int64)this->getInt8());
	return ret;
}

__int64 file_ustring::getVarInt_B()
{
	unsigned char firstByte;

	firstByte = (unsigned char)this->getInt8();

	if (firstByte < 253) {
		return firstByte;
	}
	if (firstByte == 253) {
		return ((unsigned short)this->getInt8() << 8) + (unsigned short)this->getInt8();
	}
	if (firstByte == 254) {
		return ((unsigned __int32)this->getInt8() << 24) + ((unsigned __int32)this->getInt8() << 16) + ((unsigned __int32)this->getInt8() << 8) + (unsigned __int32)this->getInt8();
	}
	if (firstByte == 255) {
		return ((unsigned __int64)this->getInt8() << 56) + ((unsigned __int64)this->getInt8() << 48) + ((unsigned __int64)this->getInt8() << 40) + ((unsigned __int64)this->getInt8() << 32) + ((unsigned __int64)this->getInt8() << 24) + ((unsigned __int64)this->getInt8() << 16) + ((unsigned __int64)this->getInt8() << 8) + (unsigned __int64)this->getInt8();
	}
	return 0;
}

__int32 file_ustring::getVarInt32()
{
	__int32 b;
	__int32 result = 0;

	for (int j = 0; j < 5; j++)
	{
		b = (unsigned __int32)this->getInt8();	result += b << 7 * j;	if (!(b & 0x80)) goto done;
		result -= 0x80;
	}

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits. max - max32 = 10 - 5 = 5
	for (int j = 0; j < 5; j++) {
		b = (unsigned __int32)this->getInt8(); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	return NULL;
done:

	return result;
}

__int64 file_ustring::getVarInt64()
{
	__int64 b;
	__int64 result = 0;

	for (int j = 0; j < 5; j++)
	{
		b = (unsigned __int64)this->getInt8();	result += b << 7 * j;	if (!(b & 0x80)) goto done;
		result -= 0x80;
	}

	// If the input is larger than 32 bits, we still need to read it all
	// and discard the high-order bits. max - max32 = 10 - 5 = 5
	for (int j = 0; j < 5; j++) {
		b = (unsigned __int64)this->getInt8(); if (!(b & 0x80)) goto done;
	}

	// We have overrun the maximum size of a varint (10 bytes).  Assume
	// the data is corrupt.
	return NULL;
done:

	return result;
}

string file_ustring::getString(int len)
{
	string ret;
	for (int i = 0; i<len; i++)
	{
		BYTE tmp = this->getInt8();
		ret += tmp;
	}
	if (ret.empty())
		return string();
	return ret;
}

ustring file_ustring::getUstring(int len)
{
	ustring ret;
	for (int i = 0; i<len; i++)
	{
		BYTE tmp = this->getInt8();
		ret += tmp;
	}
	if (ret.empty())
		return ustring();
	return ret;
}

string file_ustring::getVarString()
{
	int len = this->getVarInt32();
	string ret = this->getString(len);
	return ret;
}

ustring file_ustring::getVarUstring()
{
	int len = this->getVarInt32();
	ustring ret = this->getUstring(len);
	return ret;
}

string file_ustring::getVarString_B()
{
	int len = (int)this->getVarInt_B();
	string ret = this->getString(len);
	return ret;
}

ustring file_ustring::getVarUstring_B()
{
	int len = (int)this->getVarInt_B();
	ustring ret = this->getUstring(len);
	return ret;
}

void file_ustring::setFile(FILE * file)
{
	this->pFile = file;
}