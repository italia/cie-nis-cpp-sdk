#ifndef READER_GUARD
#define READER_GUARD

#include <vector>
#include <unordered_map>
#include "nis_types.h"
#include "token.h"

using namespace std;

namespace nis {
	namespace interface {
		class Reader
		{
		protected:
			unordered_map<string,shared_ptr<Token>> tokenList;
		public:
			virtual ~Reader() = 0;
			virtual ReaderResult enumerateReaderList() = 0;
			virtual vector<string> getReaderList() = 0;
			shared_ptr<Token> getToken(const string &name) { unordered_map<string,shared_ptr<Token>>::const_iterator tok = tokenList.find(name); if(tok != tokenList.end()) return tok->second; else return nullptr; }
		};
	}
}

#endif
