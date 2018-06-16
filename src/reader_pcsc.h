/** @filereader_pcsc.h 
 *  @brief The PCSC-specific implementation of a Reader
 *
 *  Contains the PCSC-centric implementation of a reader, managing all PCSC compatible reader. Based on libpcsc-lite.
 *  @see Reader
 *  */

#ifndef READER_PCSC_GUARD
#define READER_PCSC_GUARD

#include <winscard.h>
#include "reader.h"

using namespace cie::nis;
using namespace std;

namespace cie {
	namespace nis {
		class ReaderPCSC : public Reader
		{
		private:
			/** hold the pscs-lite context */
			SCARDCONTEXT context;
			/** multi-string containing the connected Reader's names */
			char *readerList;
		public:
			/** flag to indicate whether the pcsc context has been obtained succesfully */
			const bool hasContext = false;

			ReaderPCSC();
			~ReaderPCSC();
			ReaderResult enumerateReaderList();
			vector<string> getReaderList();
		};
	}
}

#endif
