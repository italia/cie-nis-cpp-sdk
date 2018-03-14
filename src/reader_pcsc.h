#ifndef READER_PCSC_GUARD
#define READER_PCSC_GUARD

#include <winscard.h>
#include "reader.h"

using namespace nis::interface;
using namespace std;

namespace nis
{
	namespace implementation
	{
		class ReaderPCSC : public Reader
		{
		private:
			SCARDCONTEXT context;
			char *readerList;
			bool hasContextFlag;
		public:
			ReaderPCSC();
			~ReaderPCSC();
			ReaderResult enumerateReaderList();
			vector<string> getReaderList();
			bool hasContext() {return hasContextFlag;}
		};
	}
}

#endif
