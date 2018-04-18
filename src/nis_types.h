/** @file nis_types.h
 *  @brief NIS main header
 *      
 *  Contains the return types and main types for NIS core functions.
 */

#ifndef NIS_TYPES
#define NIS_TYPES

#include <stddef.h>
#include <stdint.h>

#define NIS_LENGTH	12

/** An handler to refer to a specific instance of a token */
typedef void* NISHandle;
/** Function pointer to a callback to be called when the NIS is being read from a token
 * @param nisData pointer to the array that contains the NIS
 * @param lenData size of @e nisData
 * */
typedef void (*nis_callback_t)(char *const nisData, size_t lenData);

/** \enum Represents possible return types for ::Token methods
 */
enum TokResult { 
	TOK_RESULT_OK, /*!< Success */
	TOK_RESULT_EOF, /*!< End Of File reached */
	TOK_RESULT_NOT_CONNECTED, /*!< No card connected */
	TOK_RESULT_READ_ERROR, /*!< Cannot read from the card */
	TOK_RESULT_WRONG_LENGTH, /*!< Length is wrong */
	TOK_RESULT_OFFSET_OOB, /*!< Wrong parameter */
	TOK_RESULT_GENERIC_ERROR /*!< Some error has occurred */
};

/** \enum Represents possible return types for ::Reader methods
 */
enum ReaderResult { 
	READER_RESULT_OK, /*!< Success */
	READER_RESULT_GENERIC_ERROR /*!< Some error has occurred */
};

/** \enum Represents the various backend types as a bitfield
 */
enum BackendType { 
	NIS_BACKEND_NONE = 0, /*!< No backend selected */ 
	NIS_BACKEND_PCSC = 1, /*!< PCSC backend */
	NIS_BACKEND_ALL = (NIS_BACKEND_PCSC/* | NIS_BACKEND._...*/) /*!< All available backends */
};

/** \enum Represents the supported methods of authentication
 */
enum AuthType {
	AUTH_NONE,	/*!< Do not check */
	AUTH_PASSIVE,	/*!< Check whether the digest of the public files matches with the one in the SOD. Does not guarantee about cloning */
	AUTH_INTERNAL	/*!< Internal Authentication checks SOD digests and whether the card has been cloned */
};

/** \enum EFID of teh Elementary File in the CIE
 */
enum Efid {
	EFID_ID_SERVIZI = 0x1001,	/*!< EF.ID_Servizi, a.k.a. NIS */
	EFID_SERVIZI_INT_KPUB = 0x1005,	/*!< EF.Servizi_Int.Kpub */
	EFID_SOD = 0x1006		/*!< EF.SOD */
};

#endif
