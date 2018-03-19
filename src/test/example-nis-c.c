/** @file example-nis.cpp
 *  @brief Test example to read the NIS using the cie-nis-cpp-sdk library.
 * 
 *  This example initialize all implemented backends, enumerate the connected readers, let the user choose a reader and then try to read the NIS from the inserted token. The read is done via background polling and callback method. On exit, all initialized backends are freed.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "nis.h"

#define _TCHAR char

void deinit(void)
{
	//deinit all subsystems
	NIS_Deinit(NIS_BACKEND_ALL);
}

void callback1(char *const nisData, size_t lenData)
{
	nisData[NIS_LENGTH] = '\0';
	printf("NIS thA: %s\n", nisData);
}

void callback2(char *const nisData, size_t lenData)
{
	nisData[NIS_LENGTH] = '\0';
	printf("NIS thB: %s\n", nisData);
}

int main(int argc, _TCHAR* argv[])
{
	//init all subsystems (PCSC, ...)
	if(NIS_Init(NIS_BACKEND_ALL)) {
		printf("Error initializing backend sybsystem\n");
		exit(-1);
	}
	atexit(deinit);

	//obtain the flat list of all readers from all backends
	char *readerList = NULL;
	size_t len = 0;
	if(NIS_ReaderList(&readerList, &len)) {
		printf("Error obtaining readers list\n");
		exit(-2);
	}
	
	//helper structure to visualize and select the reader to be used
	char* reader = readerList;
	int readersSize = 0;
	//obtain the readers number
	while (reader[0]) {
		reader += strlen(reader) + 1;
		++readersSize;
	}
	reader = readerList;
	char *readers[readersSize];	
	int i = 0;
	//fill the readers list
	while (reader[0]) {
		readers[i++] = reader;
		reader += strlen(reader) + 1;
	}

	int readerNum = 1;
	//ask the user to select a reader, if more than one
	if(readersSize > 1)
	{
		for (int i = 0; i < readersSize; i++) {
			printf("(%d) %s\n", (i + 1), readers[i]);
		}
		printf("Select a reader\n");
		scanf("%d", &readerNum);
		if (readerNum < 1 || readerNum > readersSize) {
			printf("Reader not found\n");
			exit(-3);
		}
	}
	
	//obtain the handle to the specified reader
	NISHandle handle = NIS_GetHandle(readers[readerNum - 1]);
	if(handle == NULL) {
		printf("Error obtaining reader handle\n");
		exit(-4);
	}

	//read the NIS
	char nisData1[NIS_LENGTH+1];	//to take into account for the termination character
	char nisData2[NIS_LENGTH+1];	//to take into account for the termination character
	uint32_t uid1, uid2;
	if(NIS_ReadNis(handle, nisData1, callback1, 1000/*ms*/, &uid1)) {
		printf("Could not start the 1st NIS reading thread\n");
		exit(-5);
	}
	if(NIS_ReadNis(handle, nisData2, callback2, 1000/*ms*/, &uid2)) {
		printf("Could not start the 2nd NIS reading thread\n");
		exit(-6);
	}
		
	//wait while the callback read the NIS at regular interval
	for(int i = 0; i < 10; ++i)
		sleep(1);

	NIS_StopPoll(uid1);
	NIS_StopPoll(uid2);

	return 0;
}
