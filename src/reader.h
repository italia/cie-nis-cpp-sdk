/** @file reader.h
 *  @brief A Reader
 *
 *  This file contains the declaration of a base Reader. In this library, the words 'Reader' and 'Backend' are used almost interchangeably, meaning that a Reador or a Backend is a system that can exchange information with a Token (i.e. a card) via a specific protocol (e.g. PCSC compatible reader). A Reader is responsible for high-level facilities like enumerating the connected readers that are compatible with the backend it implements and to provide a reference to a named ::Token. This calss is abstract and need to be extended in order to provide a specific implementation. Usually for each Reader's subclass there is a corresponding Token's subclass.
 *  @see ReaderPCSC
 *  @see TokenPCSC
 */

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
			/** the list of the token managed by this backend */
			unordered_map<string,shared_ptr<Token>> tokenList;
		public:
			/** destructor extending this class should release all associated resources, that's why it's declared pure virtual */
			virtual ~Reader() = 0;
			/** 
			 * Add to tokenList the reader compatible with this implementation.
			 * @return ::ReaderResult indicating the success or error
			 */
			virtual ReaderResult enumerateReaderList() = 0;
			/** 
			 * Obtain the array of all compatible readers.
			 * @return The array of all the readers managed by this backend
			 */
			virtual vector<string> getReaderList() = 0;
			/** 
			 * Obtain a token given its name.
			 * @aram[in] name the name of the token as enumerated in getReaderList()
			 * @return A reference to the specified Token, or nullptr if there's no token with that name
			 */
			shared_ptr<Token> getToken(const string &name) { unordered_map<string,shared_ptr<Token>>::const_iterator tok = tokenList.find(name); if(tok != tokenList.end()) return tok->second; else return nullptr; }
		};
	}
}

#endif
