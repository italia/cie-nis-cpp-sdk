#ifndef UTIL_H_GUARD
#define UTIL_H_GUARD

#include <string>
#include <winscard.h>
#include "array.h"

#define ZeroMem(var) memset(&var,0,sizeof(var))

#define checkOutPtr(ptr) \
	if (IsBadWritePtr((ptr),sizeof(*(ptr)))) { \
		return CKR_ARGUMENTS_BAD; \
		}

#define checkInPtr(ptr) \
	if (IsBadReadPtr((ptr),sizeof(*(ptr)))) { \
		return CKR_ARGUMENTS_BAD; \
		}

//#define checkOutBuffer(ptr,size)
#define checkOutBuffer(ptr,size) \
	if (IsBadWritePtr((ptr),(size))) { \
		return CKR_ARGUMENTS_BAD; \
		}

#define checkInBuffer(ptr,size) \
	if (IsBadReadPtr((ptr),(size))) { \
		return CKR_ARGUMENTS_BAD; \
		}

#define checkInArray(ptr,size) \
	if (IsBadReadPtr((ptr),(size)*sizeof(*(ptr)))) { \
		return CKR_ARGUMENTS_BAD; \
		}

#define checkOutArray(ptr,size) \
	if (IsBadWritePtr((size),sizeof(*(size)))) { \
		return CKR_ARGUMENTS_BAD; \
		} \
	if ((ptr)!=nullptr && IsBadWritePtr((ptr),(*(size))*sizeof(*(ptr)))) { \
		return CKR_ARGUMENTS_BAD; \
		}



uint8_t hex2byte(char h);
void readHexData(const char *data,ByteDynArray &ba);
std::string HexByte(uint8_t data, bool uppercase = true);
std::string &dumpHexData(ByteArray &data, std::string &dump);
std::string &dumpHexData(ByteArray &data, std::string &dump, bool withSpace, bool uppercase = true);
std::string &dumpHexDataLowerCase(ByteArray &data, std::string &dump);

void PutPaddingBT0(ByteArray &ba, size_t dwLen);
void PutPaddingBT1(ByteArray &ba, size_t dwLen);
void PutPaddingBT2(ByteArray &ba, size_t dwLen);
size_t RemovePaddingBT1(ByteArray &paddedData);
size_t RemovePaddingBT2(ByteArray &paddedData);
size_t RemoveISOPad(ByteArray &paddedData);

size_t RemoveSha1(ByteArray &paddedData);
size_t RemoveSha256(ByteArray &paddedData);

size_t ANSIPadLen(size_t Len);
void ANSIPad(ByteArray &Data, size_t DataLen);
size_t ISOPadLen(size_t Len);
void ISOPad(const ByteArray &Data, size_t DataLen);
long ByteArrayToInt(ByteArray &ba);
const ByteDynArray ISOPad(const ByteArray &data);
const ByteDynArray ISOPad16(const ByteArray &data);

ByteDynArray ASN1Tag(DWORD tag,ByteArray &content);

#endif
