#include "nis.h"

int NIS_Init(uint32_t backendBitfield)
{
	return 0;
}

int NIS_ReaderList(char **readers)
{
	return 0;
}

NISHandle NIS_GetHandle(char *readerName)
{
	return 0;
}

int NIS_ReadNis(NISHandle handle, char *const nisData, uint16_t lenData, nis_callback_t callback)
{
	return 0;
}

int NIS_ConfigHandle(NISHandle handle, uint32_t config)
{
	return 0;
}

int NIS_Deinit(uint32_t backendBitfield)
{
	return 0;
}

int NIS_Reset(NISHandle handle)
{
	return 0;
}
