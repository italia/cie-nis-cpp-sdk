#ifndef TOKEN_GUARD
#define TOKEN_GUARD

#include <vector>
#include <winscard.h>
#include "nis_types.h"

namespace nis {
	namespace interface {
		class Token
		{
		public:
			virtual ~Token() = 0;
			virtual TokResult connect() = 0;
			virtual TokResult disconnect() = 0;
			virtual TokResult transmit(const std::vector<BYTE> &apdu, std::vector<BYTE> &response) const = 0;
		};
	}
}

#endif
