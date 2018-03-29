#ifndef NIS_GUARD
#define NIS_GUARD

#ifdef  __cplusplus
#include <vector>
#include <string>
#include "token.h"
#endif
#include <stdint.h>
#include "nis_types.h"

#ifdef  __cplusplus
extern "C" {
#endif
	int NIS_Init(uint32_t backendBitfield);
	int NIS_ReaderList(char **readers, size_t *len);
	NISHandle NIS_GetHandle(char *readerName);
	int NIS_ReadNis(NISHandle handle, char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid, enum AuthType auth);
	int NIS_ConfigHandle(NISHandle handle, uint32_t config);
	int NIS_Deinit(uint32_t backendBitfield);
	int NIS_Reset(NISHandle handle);
	int NIS_StopPoll(uint32_t uid);
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
#include "token.h"

namespace cie {
	namespace nis {
		int init(uint32_t backendBitfield);
		std::vector<std::string> readersList();
		cie::nis::Token* getToken(std::string readerName);
		int stopPoll(uint32_t uid);
		int deinit(uint32_t backendBitfield);
	}
}
#endif

#endif

