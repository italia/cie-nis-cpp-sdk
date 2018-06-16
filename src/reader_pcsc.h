/** @filereader_pcsc.h
 *  @brief The PCSC-specific implementation of a Reader
 *
 *  Contains the PCSC-centric implementation of a reader, managing all PCSC
 * compatible reader. Based on libpcsc-lite.
 *  @see Reader
 *  */

#ifndef READER_PCSC_GUARD
#define READER_PCSC_GUARD

#include <winscard.h>
#include <string>
#include <vector>
#include "reader.h"

namespace cie {
namespace nis {

class ReaderPCSC : public Reader {
 private:
  // hold the pscs-lite context
  SCARDCONTEXT context;

  // multi-string containing the connected Reader's names
  std::vector<std::string> readerList;

 public:
  // flag to indicate whether the pcsc context has been obtained successfully
  const bool hasContext = false;

  ReaderPCSC();
  ~ReaderPCSC();
  ReaderResult enumerateReaderList();
  std::vector<std::string> getReaderList();
};

}  // namespace nis
}  // namespace cie

#endif  // READER_PCSC_GUARD
