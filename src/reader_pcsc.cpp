#include <iostream>
#include <sstream>
#include <vector>
#include <ios>
#include "token_pcsc.h"
#include "reader_pcsc.h"
#include <cstring>

using namespace nis::implementation;

ReaderPCSC::ReaderPCSC() : readerList{nullptr}
{
	hasContextFlag = (SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context) == SCARD_S_SUCCESS);
}

ReaderPCSC::~ReaderPCSC()
{
	if(readerList) 
		SCardFreeMemory(context, readerList); 

	if(hasContextFlag)
		SCardReleaseContext(context);
}

vector<string> ReaderPCSC::getReaderList()
{
	vector<string> v;

	char* reader = readerList;
	while (reader[0]) {
		string name{reader};
		v.push_back(name);
		reader += strlen(reader) + 1;
	}

	return v;
}

ReaderResult ReaderPCSC::enumerateReaderList()
{
	tokenList.clear();

	DWORD ReaderListLen = SCARD_AUTOALLOCATE;
	if(readerList)
		SCardFreeMemory(context, readerList);
	SCardListReaders(context, NULL, (char*) &readerList, &ReaderListLen);

	char* reader = readerList;
	while (reader[0]) {
		string name{reader};
		tokenList[name] = shared_ptr<TokenPCSC>{new TokenPCSC{name, context}};
		reader += strlen(reader) + 1;
	}

	return READER_RESULT_OK;
}
