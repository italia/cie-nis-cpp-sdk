#ifndef NIS_GUARD
#define NIS_GUARD

#include <stdint.h>
#include "nis_types.h"

extern "C"
{
	int NIS_Init(uint32_t backendBitfield);
	int NIS_ReaderList(char **readers);
	NISHandle NIS_GetHandle(char *readerName);
	int NIS_ReadNis(NISHandle handle, char *const nisData, uint16_t lenData, nis_callback_t callback);
	int NIS_ConfigHandle(NISHandle handle, uint32_t config);
	int NIS_Deinit(uint32_t backendBitfield);
	int NIS_Reset(NISHandle handle);
}

#endif

