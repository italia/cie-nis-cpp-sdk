/** @file request.h
 *  @brief Request helper functions 
 *
 *  This file contains all the functions that abstract the high level protocol involving APDUs.
 *  @see Token
 */

#ifndef REQUESTS_GUARD
#define REQUESTS_GUARD

#include "token.h"

using namespace cie::nis;

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
 * Sends a request to read NIS from the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] response coming from the card
 * @return in case of a successful reply, true is returned, false otherwise.
 */
bool read_nis(const Token &card, std::vector<BYTE> &response);

/**
 * Sends a request to read EF.SOD content from the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] response coming from the card
 * @return true if SOD has been read, false otehrwise
 */
bool read_sod(const Token &card, std::vector<BYTE> &response);

/**
 * Sends a request to read EF.Servizi_Int.Kpub content from the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] ret coming from the card
 * @return true if the file has been read, false otehrwise
 */
bool read_service_int_kpub(const Token &card, std::vector<BYTE> &response);

/**
 * Prompts the user for single APDU values and stores the newly
 * created APDU in the apdu vector. Fields must be separated
 * by a '-'.
 * @param[out] apdu the payload created from the inserted values
 * @return on success, true is returned, false otherwise.
 */
bool create_apdu(std::vector<BYTE> &apdu);

/**
 * Sends the MSE SET command to the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[out] response is the answer coming from from the card
 * @return true if the command was successful, false otherwise
 */
bool mse_set(const Token &card, std::vector<BYTE> &response);

/**
 * Sends the INTERNAL AUTHENTICATE command and the relative challenge to the specified card handle.
 * @param[in] card a reference to token involved in the transaction
 * @param[in] challenge contains the array against which the card is challenged. Must be less than or equal to 256 bytes in size.
 * @param[out] response is the asnwer (response to the challenge) coming from from the card
 * @return true if the command was successful, false otherwise
 */
bool internal_authenticate(const Token &card, const std::vector<BYTE> &challenge, std::vector<BYTE> &response);

bool start_interactive_session(const Token &card);
}
#endif
