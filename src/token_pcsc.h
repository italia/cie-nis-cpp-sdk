#ifndef TOKEN_PCSC_GUARD
#define TOKEN_PCSC_GUARD

#include <winscard.h>
#include "token.h"

using namespace cie::nis;
using namespace std;

namespace cie {
	namespace nis {
		const std::size_t RESPONSE_SIZE {300};

		class TokenPCSC : public Token
		{
		private:
			string identifier;
			bool isConnected;
			SCARDHANDLE card;
			SCARDCONTEXT context;
		public:
			TokenPCSC(const string &identifier, SCARDCONTEXT context);
			~TokenPCSC();
			TokResult connect();
			TokResult disconnect();
			TokResult transmit(const std::vector<BYTE> &apdu, std::vector<BYTE> &response) const;
		};
	}
}

#endif
