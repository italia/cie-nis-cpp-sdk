/** @file example-nis.cpp
 *  @brief Test example to read the NIS using the cie-nis-cpp-sdk library.
 * 
 *  This example initialize all implemented backends, enumerate the connected readers, let the user choose a reader and then try to read the NIS from the inserted token. The read is done via background polling and callback method. On exit, all initialized backends are freed.
 */

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include "nis.h"

#define _TCHAR char

using namespace std;

#define F(x)    (x)

class LoggerUtil
{
public:
	static void  print(const std::string &s) { std::cout << s; }
	static void  println(const std::string &s) { print(s); std::cout << std::endl; }
	static void  print(const int i) { print(std::to_string(i)); }
	static void  println(const int i) { println(std::to_string(i)); }
} Serial;

void printNameForType(const byte classification, const byte type) {

  if (classification > 0 ) {
    switch (classification) {
      case 1:
        Serial.print(F("APPLICATION"));
        return;
      case 2: 
        //TODO: return the actual offset
        Serial.print(F("["));
        Serial.print(type);
        Serial.print(F("]"));
        return;
      case 3:
      Serial.print(F("PRIVATE"));
      return;
    }
  }

  switch(type) {
    case 0x00:
      Serial.print(F("END OF CONTENT"));
      break;
    case 0x01:
      Serial.print(F("BOOLEAN"));
      break;
    case 0x02:
      Serial.print(F("INTEGER"));
      break;
    case 0x03:
      Serial.print(F("BIT STRING"));
      break;
    case 0x04:
      Serial.print(F("OCTET STRING"));
      break;
    case 0x05:
      Serial.print(F("NULL"));
      break;
    case 0x06:
      Serial.print(F("OBJECT IDENTIFIER"));
      break;
    case 0x07:
      Serial.print(F("OBJECT DESCRIPTOR"));
      break;
    case 0x08:
      Serial.print(F("EXTERNAL"));
      break;
    case 0x09:
      Serial.print(F("REAL"));
      break;
    case 0x0A:
      Serial.print(F("ENUMERATED"));
      break;
    case 0x0B:
      Serial.print(F("EMBEDDED PDV"));
      break;
    case 0x0C:
      Serial.print(F("UTF8 STRING"));
      break;
    case 0x0D:
      Serial.print(F("RELATIVE OID"));
      break;
    case 0x10:
      Serial.print(F("SEQUENCE"));
      break;
    case 0x11:
      Serial.print(F("SET"));
      break;
    case 0x12:
      Serial.print(F("NUMERIC STRING"));
      break;
    case 0x13:
      Serial.print(F("PRINTABLE STRING"));
      break;
    case 0x14:
      Serial.print(F("T61 STRING"));
      break;
    case 0x15:
      Serial.print(F("VIDEOTEX STRING"));
      break;
    case 0x16:
      Serial.print(F("IA5 STRING"));
      break;
    case 0x17:
      Serial.print(F("UTC TIME"));
      break;
    case 0x18:
      Serial.print(F("GENERALIZED TIME"));
      break;
    case 0x19:
      Serial.print(F("GRAPHIC STRING"));
      break;
    case 0x1A:
      Serial.print(F("VISIBLE STRING"));
      break;
    case 0x1B:
      Serial.print(F("GENERAL STRING"));
      break;
    case 0x1C:
      Serial.print(F("UNIVERSAL STRING"));
      break;
    case 0x1D:
      Serial.print(F("CHARACTER STRING"));
      break;
    case 0x1E:
      Serial.print(F("BMP STRING"));
      break;
    default:
      Serial.print(F("UNKNOWN"));
      break;
  }
}

bool printTriple(cie_BerTriple *triple, void *data) {
  if (triple->depth > 0) {
    for (byte i = 0; i < triple->depth; i++) {
      Serial.print("  ");
    }
  }

  printNameForType(triple->classification, triple->type);
  Serial.print(" (offset: ");
  Serial.print(triple->offset);
  Serial.print(", length: ");
  Serial.print(triple->contentOffset - triple->offset);
  Serial.print("+");
  Serial.print(triple->contentLength);   
  
  if(triple->contentLength <= 32) 
  {
	  Serial.print(", ");
	  for(int h = 0; h < triple->contentLength; ++h) 
	  {
		Serial.print("\u001B[32m");
		char hex[4];
	  	sprintf(hex, "%02X ", triple->value[h]);
	       	Serial.print(hex);
	       	Serial.print("\u001B[0m");
	  }
  } 
  else Serial.print(", ...");

  if (triple->encoding == 0x01) {
    Serial.println(", constructed)");
  } else {
    Serial.println(", primitive)");
  }

  return true;

}

void deinit(void)
{
	//deinit all subsystems
	cie::nis::deinit(NIS_BACKEND_ALL);
}

int main(int argc, _TCHAR* argv[])
{
	//init all subsystems (PCSC, ...)
	if(cie::nis::init(NIS_BACKEND_ALL)) {
		std::cerr << "Error initializing backend sybsystem" << std::endl;
		exit(-1);
	}
	atexit(deinit);

	//obtain the flat list of all readers from all backends
	vector<string> readers = cie::nis::readersList();
	
	int readerNum = 1;
	//ask the user to select a reader, if more than one is present
	if(readers.size() > 1)
	{
		for (int i = 0; i < readers.size(); i++) {
			std::cout << (i + 1) << ") " << readers[i] << std::endl;
		}
		std::cout << "Select a reader" << std::endl;
		std::cin >> readerNum;
		if (readerNum < 1 || readerNum > readers.size()) {
			std::cerr << "Reader not found" << std::endl;
			exit(-3);
		}
	}
	
	//obtain the handle to the specified reader
	cie::nis::Token* token = cie::nis::getToken(readers[readerNum - 1]);
	if(token == nullptr) {
		std::cerr << "Error obtaining reader handle" << std::endl;
		exit(-4);
	}

	//read the NIS
	std::cout << "NIS: " << token->readNis(AUTH_PASSIVE) << std::endl;

	//read the EF.SOD
	cie::nis::Sod sod;
	if(token->parseSod(printTriple, &sod)) {
		std::cerr << "Could not read the SOD" << std::endl;
		exit(-5);
	}

	return 0;
}
