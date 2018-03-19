/**@file token.h
 * @brief A Token
 *
 * A Token represents a card or token with which a Reader can interact. It's main use is to connect/disconnect and to transmit/receive data packets to/from a card. This is an abstract class meaning that it must be extended to provide the funcionality of a specific protocol (e.g. PCSC). Usually for each Token's subclass there is a corresponding Reader's subclass.
 * @see TokenPCSC
 * @see ReaderPCSC
 */

#ifndef TOKEN_GUARD
#define TOKEN_GUARD

#include <vector>
#include <memory>
#include <winscard.h>
#include "nis_types.h"

namespace cie {
	namespace nis {
		class Token
		{
		public:
			virtual ~Token() = 0;
			int readNis(char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid);
			std::string readNis();
			int configure(uint32_t config);
			int reset();
		public:	//this should really be private
			/** 
			 * Connect to this instance of a token 
			 * @return ::TokResult representing the success or error conditiona
			 * @see disconnect()
			 */
			virtual TokResult connect() = 0;
			/** 
			 * Disconnect to this instance of a token 
			 * @return ::TokResult representing the success or error conditiona
			 * @see connect()
			 */
			virtual TokResult disconnect() = 0;
			/** 
			 * Transmit a specific apdu to a card and obtain its response
			 * @param[in] array containing the apdu to be sent to the token
			 * @param[out] the response sent from the token
			 * @return ::TokResult representing the success or error condition
			 */
			virtual TokResult transmit(const std::vector<BYTE> &apdu, std::vector<BYTE> &response) const = 0;
		};
	}
}

#endif
