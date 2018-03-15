#ifndef NIS_GUARD
#define NIS_GUARD

#include <stdint.h>
#include "nis_types.h"

extern "C"
{
	int NIS_Init(uint32_t backendBitfield);
	int NIS_ReaderList(char **readers, size_t *len);
	NISHandle NIS_GetHandle(char *readerName);
	int NIS_ReadNis(NISHandle handle, char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid);
	int NIS_ConfigHandle(NISHandle handle, uint32_t config);
	int NIS_Deinit(uint32_t backendBitfield);
	int NIS_Reset(NISHandle handle);
	int NIS_StopPoll(uint32_t uid);
}

#endif

