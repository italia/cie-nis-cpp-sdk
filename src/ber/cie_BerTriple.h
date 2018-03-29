/**************************************************************************/
/*! 
    @file     cie_BerTriple.h
    @author   Developers Italia
	@license  BSD (see License)
	
	Definition of the cie_BerTriple structure used to describe an individual piece of information in BER encoded binary content
	
	@section  HISTORY

	v1.0  - First definition of the structure
	
*/
/**************************************************************************/
#ifndef CIE_BER_TRIPLE
#define CIE_BER_TRIPLE

#include "stdint.h"

typedef unsigned int word;
typedef uint8_t byte;
typedef byte cie_EFPath;
namespace cie {
	namespace nis {
		class Token;
	}
}
using cie_PN532 = cie::nis::Token;


class cie_BerTriple {
  public:
	byte classification;
	byte encoding;
	unsigned int type;
	word offset;
	word contentOffset;
	word contentLength;
	byte depth;
	byte *value;
};
typedef bool (*cieBerTripleCallbackFunc)(cie_BerTriple*, void*);
#endif
