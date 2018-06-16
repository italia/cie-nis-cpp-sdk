#include <iostream>
#include <sstream>
#include <vector>
#include <ios>
#include "token_pcsc.h"
#include "reader_pcsc.h"
#include <cstring>

using namespace cie::nis;

ReaderPCSC::ReaderPCSC()
  : hasContext{SCardEstablishContext(
        SCARD_SCOPE_SYSTEM, nullptr, nullptr, &context) == SCARD_S_SUCCESS}
{
}

ReaderPCSC::~ReaderPCSC()
{
	if(hasContext)
		SCardReleaseContext(context);
}

vector<string> ReaderPCSC::getReaderList()
{
	return readerList;
}

ReaderResult ReaderPCSC::enumerateReaderList()
{
	readerList.clear();
	tokenList.clear();

	DWORD dwReaders;
	if (SCardListReaders(context, nullptr, nullptr, &dwReaders) != SCARD_S_SUCCESS)
		return READER_RESULT_GENERIC_ERROR;

	char mszReaders[dwReaders] = {'\0'};
	if (SCardListReaders(context, nullptr, mszReaders, &dwReaders) != SCARD_S_SUCCESS)
		return READER_RESULT_GENERIC_ERROR;

	char* reader = mszReaders;
	while (*reader) {
		string name{reader};
		reader += strlen(reader) + 1;

		readerList.emplace_back(name);
		tokenList.emplace(name, std::make_shared<TokenPCSC>(name, context));
	}

	return READER_RESULT_OK;
}
