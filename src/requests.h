#ifndef REQUESTS_GUARD
#define REQUESTS_GUARD

#include "token.h"

using namespace nis::interface;

namespace Requests
{
/*
 * Sends the specified apdu, which must be encapsulated in a BYTE vector,
 * to the specified card handle.
 * The response is stored in the response vector.
 * In case of a successful reply, true is returned, false otherwise.
 */
bool send_apdu(const Token &card, const std::vector<BYTE> &apdu,
		std::vector<BYTE> &response);

/*
 * Sends a request to select file by AID to the specified card handle.
 * The response is stored in the response vector.
 * In case of a successful reply, true is returned, false otherwise.
 */
bool select_df_ias(const Token &card, std::vector<BYTE> &response);

/*
 * Sends a request to select CIE object DF to the specified card handle.
 * The response is stored in the response vector.
 * In case of a successful reply, true is returned, false otherwise.
 */
bool select_cie_df(const Token &card, std::vector<BYTE> &response);

/*
 * Sends a request to read NIS from the specified SCARDHANDLE.
 * Response is stored in the response vector.
 *
 * In case of a successful reply, true is returned, false otherwise.
 */
bool read_nis(const Token &card, std::vector<BYTE> &response);

/*
 * Propts the user for single APDU values and stores the newly
 * created APDU in the apdu vector. Fields must be separated
 * by a '-'.
 * On success, true is returned.
 */
bool  create_apdu(std::vector<BYTE> &apdu);

bool start_interactive_session(const Token &card);
}
#endif
