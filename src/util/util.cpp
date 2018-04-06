#include <vector>
#include <iomanip>
#include <sstream>
#include "util.h"


BYTE hex2byte(char h) {
	if (h>='0' && h<='9') return(h-'0');
	if (h>='A' && h<='F') return(h+10-'A');
	if (h>='a' && h<='f') return(h+10-'a');
	return(0);
}

bool ishexdigit(char c) {
	if (c>='0' && c<='9') return true;
	if (c>='a' && c<='f') return true;
	if (c>='A' && c<='F') return true;
	return false;
}

size_t countHexData(const char *data)
{
	size_t cnt = 0;
	size_t slen=strlen(data);
	for (size_t i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  logged_error("Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  logged_error("richiesto spazio");
		}
		cnt++;

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}
	return cnt;
}

size_t setHexData(const char *data, uint8_t *buf)
{
	size_t cnt = 0;
	size_t slen=strlen(data);
	for (size_t i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  logged_error("Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  logged_error("richiesto spazio");
		}
		buf[0]=v;
		buf++;
		cnt++;

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}
	return cnt;
}

void readHexData(const char *data,ByteDynArray &ba)
{
	std::vector<uint8_t> dt;


	size_t slen=strlen(data);
	for (size_t i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  logged_error("Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  logged_error("richiesto spazio");
		}
		dt.push_back(v);

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}

	if (dt.size()>0)
		ba = ByteArray(&dt[0], dt.size());
	else 
		ba.clear();
}

std::string HexByte(uint8_t data, bool uppercase) {
	std::stringstream dmp;
	dmp << std::hex << std::setfill('0');
	if (uppercase)
		dmp << std::uppercase;
	dmp << std::setw(2) << static_cast<unsigned>(data);
	return dmp.str();
}

std::string &dumpHexData(ByteArray &data, std::string &dump)
{
	dumpHexData(data,dump,true,true);
	return dump;
}

std::string &dumpHexData(ByteArray &data, std::string &dump, bool withSpace, bool uppercase)
{
	std::stringstream dmp;
	dmp << std::hex << std::setfill('0');
	if (uppercase)
		dmp << std::uppercase;
	for (size_t i = 0; i<data.size(); i++) {
		dmp << std::setw(2) << static_cast<unsigned>(data[i]);
		if (withSpace)
			dmp << " ";
	}
	dump = dmp.str();
	return dump;
}

/*void Debug(ByteArray ba) {
	std::string out;
	dumpHexData(ba,out);
	OutputDebugString(out.c_str());
	OutputDebugString("\n");
}*/

std::string &dumpHexDataLowerCase(ByteArray &data, std::string &dump)
{
	return dumpHexData(data, dump, false, false);
}

void PutPaddingBT0(ByteArray &ba, size_t dwLen)
{
	//init_func

	if (dwLen>ba.size())
		throw logged_error("Lunghezza del padding errata");

	ba.left(ba.size()-dwLen).fill(0);
	//exit_func
}


void PutPaddingBT1(ByteArray &ba, size_t dwLen)
{
	//init_func
	if (dwLen>ba.size()-3)
		throw logged_error("Lunghezza del padding errata");

	ba[0]=0;
	ba[1]=1;
	ba.mid(2,ba.size()-dwLen-3).fill(0xff);
	ba[ba.size()-dwLen-1]=0;
	//exit_func
}

void PutPaddingBT2(ByteArray &ba, size_t dwLen)
{
	//init_func
	if (dwLen>ba.size()-3)
		throw logged_error("Lunghezza del padding errata");

	ba[0]=0;
	ba[1]=2;
	ba.mid(2, ba.size() - dwLen - 3).random();
	ba[ba.size() - dwLen - 1] = 0;
	//exit_func
}

size_t RemoveSha1(ByteArray &paddedData) {
	static uint8_t SHA1Algo[] = { 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 };
	if (paddedData.left(sizeof(SHA1Algo)) == VarToByteArray(SHA1Algo))
		return sizeof(SHA1Algo);
	throw logged_error("OID Algoritmo SHA1 non presente");
}

size_t RemoveSha256(ByteArray &paddedData) {
	static uint8_t SHA256Algo[] = { 0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
	if (paddedData.left(sizeof(SHA256Algo)) == VarToByteArray(SHA256Algo))
		return sizeof(SHA256Algo);
	throw logged_error("OID Algoritmo SHA256 non presente");
}

size_t RemovePaddingBT1(ByteArray &paddedData)
{
	//init_func
	if (paddedData[0]!=0)
		throw logged_error("Errore nel padding");
	if (paddedData[1]!=1)
		throw logged_error("Errore nel padding");
	for (size_t i = 2; i<paddedData.size(); i++) {
		if (paddedData[i]==0) {
			return i+1;
		}
		if (paddedData[i]!=0xff)
			throw logged_error("Errore nel padding");
	}
	throw logged_error("Errore nel padding");
	//exit_func
}

size_t RemovePaddingBT2(ByteArray &paddedData)
{
	//init_func
	if (paddedData[0]!=0)
		throw logged_error("Errore nel padding");
	if (paddedData[1]!=2)
		throw logged_error("Errore nel padding");
	for (size_t i = 2; i<paddedData.size(); i++)
		if (paddedData[i]==0) {
			return i+1;
		}
	throw logged_error("Errore nel padding");
	//exit_func
}

size_t RemoveISOPad(ByteArray &paddedData)
{
	//init_func
		for (size_t i = paddedData.size() - 1; i >= 0; i--) {
		if (paddedData[i]!=0) {
			if (paddedData[i]!=0x80) {
				throw logged_error("Errore nel padding");
			}
			else {
				return i;
			}
		}
	}
	throw logged_error("Errore nel padding");
	//exit_func
}

size_t ANSIPadLen(size_t Len)
{
	if ((Len & 0x7) == 0)
		return(Len);
	else
		return(Len - (Len & 0x7) + 0x08);
}

void ANSIPad(ByteArray &Data, size_t DataLen)
{
	//init_func
	Data.mid(DataLen).fill(0);
	//exit_func

}

size_t ISOPadLen16(size_t Len)
{
	if ((Len & 0x10f) == 0)
		return(Len + 16);
	else
		return(Len - (Len & 0x0f) + 0x10);
}

size_t ISOPadLen(size_t Len)
{
	if ((Len & 0x7) == 0)
		return(Len+8);
	else
		return(Len - (Len & 0x7) + 0x08);
}

void ISOPad(const ByteArray &Data, size_t DataLen)
{
	//init_func
	Data.mid(DataLen).fill(0);
	Data[DataLen]=0x80;
	//exit_func
}

const ByteDynArray ISOPad16(const ByteArray &data) {
	//init_func
	ByteDynArray resp(ISOPadLen16(data.size()));
	resp.copy(data);
	ISOPad(resp, data.size());
	return resp;
	//exit_func
}

const ByteDynArray ISOPad(const ByteArray &data) {
	//init_func
	ByteDynArray resp(ISOPadLen(data.size()));
	resp.copy(data);
	ISOPad(resp,data.size());
	return resp;
	//exit_func
}

long ByteArrayToInt(ByteArray &ba)
{
	//init_func
	long val=0;
	for (size_t i = 0; i<ba.size(); i++) {
		val<<=8;
		val|=ba[i];
	}
	return val;
	//exit_func
}

ByteDynArray ASN1Tag(DWORD tag,ByteArray &content) {
	ByteDynArray result = content.getASN1Tag(tag);
	return result;
}

size_t ASN1TLength(unsigned int tag) {
	int tLen=0;
	while (tag!=0) {
		tLen++;
		tag>>=8;
	}
	return tLen;
}

void putASN1Tag(unsigned int tag, ByteArray &data) {
	int tPos=0;
	while (tag!=0) {
		uint8_t b = tag >> 24;
		if (b!=0) {
			data[tPos]=b;
			tPos++;
		}
		tag<<=8;
	}
}

size_t ASN1LLength(size_t len) {
	if (len<0x80)
		return 1;
	else {
		// vediamo quanti byte mi servono...
		if (len<=0xff)
			return 2;
		else if (len<=0xffff)
			return 3;
		else if (len<=0xffffff)
			return 4;
		else if (len<=0xffffffff)
			return 5;
	}
	throw logged_error("Lunghezza ASN1 non valida");
}

void putASN1Length(size_t len, const ByteArray &data) {
	if (len < 0x80) {
		data[0] = (uint8_t)len;
	}
	else {
		if (len <= 0xff) {
			data[0] = (0x81);
			data[1] = (uint8_t)(len);
		}
		else if (len <= 0xffff) {
			data[0] = (0x82);
			data[1] = (uint8_t)(len >> 8);
			data[2] = (len & 0xff);
		}
		else if (len <= 0xffffff) {
			data[0] = (0x83);
			data[1] = (uint8_t)(len >> 16);
			data[2] = ((len >> 8) & 0xff);
			data[3] = (len & 0xff);
		}
		else if (len <= 0xffffffff) {
			data[0] = (0x84);
			data[1] = (uint8_t)(len >> 24);
			data[2] = ((len >> 16) & 0xff);
			data[3] = ((len >> 8) & 0xff);
			data[4] = (len & 0xff);
		}
	}
}
