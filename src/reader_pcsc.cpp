#include <iostream>
#include <sstream>
#include <vector>
#include <ios>
#include "token_pcsc.h"
#include "reader_pcsc.h"
#include <cstring>

using namespace nis::implementation;

ReaderPCSC::ReaderPCSC()
{
	SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context);
}

ReaderPCSC::~ReaderPCSC()
{
	if(readerList) 
		SCardFreeMemory(context, readerList); 

	SCardReleaseContext(context);
	freeTokenList();
}

void ReaderPCSC::freeTokenList()
{
	for(auto it = tokenList.begin(); it != tokenList.end(); ++it)
		delete it->second;
}

ReaderResult ReaderPCSC::obtainReaderList()
{
	freeTokenList();
	tokenList.clear();

	DWORD ReaderListLen = SCARD_AUTOALLOCATE;
	if(readerList)
		SCardFreeMemory(context, readerList);
	SCardListReaders(context, NULL, (char*) &readerList, &ReaderListLen);

	char* reader = readerList;
	while (reader[0]) {
		string name{reader};
		tokenList[name] = new TokenPCSC{name, context};
		reader += strlen(reader) + 1;
	}

	return READER_RESULT_OK;
}
