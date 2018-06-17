#include <iostream>
#include <sstream>
#include <vector>
#include <ios>
#include "token_pcsc.h"
#include "reader_pcsc.h"
#include <cstring>

using namespace cie::nis;

ReaderPCSC::ReaderPCSC()
	: hasContext{SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context) == SCARD_S_SUCCESS}
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

	DWORD ReaderListLen = 0;
	if (SCardListReaders(context, NULL, NULL, &ReaderListLen) != SCARD_S_SUCCESS)
		return READER_RESULT_GENERIC_ERROR;

	vector<char> buffer(ReaderListLen);
	if (SCardListReaders(context, NULL, buffer.data(), &ReaderListLen) != SCARD_S_SUCCESS)
		return READER_RESULT_GENERIC_ERROR;

	char* reader = buffer.data();
	while (*reader) {
		string name{reader};
		reader += name.length() + 1;

		readerList.emplace_back(name);
		tokenList.emplace(name, make_shared<TokenPCSC>(name, context));
	}

	return READER_RESULT_OK;
}
