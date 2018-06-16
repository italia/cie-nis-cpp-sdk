#include "reader_pcsc.h"
#include <cstring>
#include "token_pcsc.h"

ReaderPCSC::ReaderPCSC()
    : hasContext{SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr,
                                       &context) == SCARD_S_SUCCESS} {}

ReaderPCSC::~ReaderPCSC() {
  if (hasContext) SCardReleaseContext(context);
}

std::vector<std::string> ReaderPCSC::getReaderList() { return readerList; }

ReaderResult ReaderPCSC::enumerateReaderList() {
  readerList.clear();
  tokenList.clear();

  DWORD dwReaders;
  if (SCardListReaders(context, nullptr, nullptr, &dwReaders) !=
      SCARD_S_SUCCESS)
    return READER_RESULT_GENERIC_ERROR;

  std::vector<char> mszReaders(dwReaders);
  if (SCardListReaders(context, nullptr, mszReaders.data(), &dwReaders) !=
      SCARD_S_SUCCESS)
    return READER_RESULT_GENERIC_ERROR;

  char* reader = mszReaders.data();
  while (*reader) {
    std::string name{reader};
    reader += strlen(reader) + 1;

    readerList.emplace_back(name);
    tokenList.emplace(name, std::make_shared<TokenPCSC>(name, context));
  }

  return READER_RESULT_OK;
}
