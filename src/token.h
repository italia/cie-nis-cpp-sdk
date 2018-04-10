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
#include <map>
#include <winscard.h>
#include "nis_types.h"
#include "util/array.h"

namespace cie {
	namespace nis {
		class Token
		{
		public:
			virtual ~Token() = 0;
			int readNis(char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid, AuthType auth);
			std::string readNis(AuthType auth);
			bool verifySod(ByteArray &SOD, std::map<BYTE, ByteDynArray> &hashSet);
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
			 * @param[in] apdu array containing the apdu to be sent to the token
			 * @param[out] response the response sent from the token
			 * @param[out] retLen filled with the number of bytes read back
			 * @return ::TokResult representing the success or error condition
			 */
			virtual TokResult transmit(const std::vector<BYTE> &apdu, std::vector<BYTE> &response, size_t *retlen=nullptr) const = 0;
			/** 
			 * Read the binary content of the speficied file
			 * @param[in] filePath SFI of the file to be read or 0 if it's already been selected through EFID
			 * @param[out] contentBuffer the content of the file
			 * @param[in] startingOffset the offset from which to start reading
			 * @param[in] contentLength the length to be read back, or -1 to read all
			 * @return true on success, false on error
			 */
			bool readBinaryContent(const uint16_t filePath, std::vector<BYTE> &contentBuffer, size_t startingOffset, size_t contentLength);
		};
	}
}

#endif
