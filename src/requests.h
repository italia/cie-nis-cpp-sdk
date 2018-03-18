/** @file request.h
 *  @brief Request helper functions 
 *
 *  This file contains all the functions that abstract the high level protocol involving APDUs.
 *  @see Token
 */

#ifndef REQUESTS_GUARD
#define REQUESTS_GUARD

#include "token.h"

using namespace nis::interface;

namespace Requests
{
/**
 * Sends the specified apdu, which must be encapsulated in a BYTE vector,
 * to the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[in] apdu the payload to be sent to the card
 * @param[out] response coming from the card
 * @return in case of a successful reply, true is returned, false otherwise.
 */
bool send_apdu(const Token &card, const std::vector<BYTE> &apdu,
		std::vector<BYTE> &response);

/**
 * Sends a request to select file by AID to the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] response coming from the card
 * @return in case of a successful reply, true is returned, false otherwise.
 */
bool select_df_ias(const Token &card, std::vector<BYTE> &response);

/**
 * Sends a request to select CIE object DF to the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] response coming from the card
 * @return in case of a successful reply, true is returned, false otherwise.
 */
bool select_df_cie(const Token &card, std::vector<BYTE> &response);

/**
 * Sends a request to read NIS from the specified SCARDHANDLE.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] response coming from the card
 * @return in case of a successful reply, true is returned, false otherwise.
 */
bool read_nis(const Token &card, std::vector<BYTE> &response);

/**
 * Prompts the user for single APDU values and stores the newly
 * created APDU in the apdu vector. Fields must be separated
 * by a '-'.
 * @param[out] apdu the payload created from the inserted values
 * @return on success, true is returned, false otherwise.
 */
bool create_apdu(std::vector<BYTE> &apdu);

bool start_interactive_session(const Token &card);
}
#endif