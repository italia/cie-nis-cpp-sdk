#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include "nis.h"

#define _TCHAR char

void deinit(void)
{
	//deinit all subsystems
	NIS_Deinit(NIS_BACKEND_ALL);
}

void callback(char *const nisData, size_t lenData)
{
	std::cout << "NIS: " << std::string{nisData} << std::endl;
}

int main(int argc, _TCHAR* argv[])
{
	//init all subsystems (PCSC, ...)
	if(NIS_Init(NIS_BACKEND_ALL)) {
		std::cerr << "Error initializing backend sybsystem" << std::endl;
		exit(-1);
	}
	atexit(deinit);

	//obtain the flat list of all readers from all backends
	char *readerList = nullptr;
	size_t len = 0;
	if(NIS_ReaderList(&readerList, &len)) {
		std::cerr << "Error obtaining readers list" << std::endl;
		exit(-2);
	}
	
	//helper structure to visualize and select the reader to be used
	char* reader = readerList;
	std::vector<char*> readers;
	while (reader[0]) {
		readers.push_back(reader);
		reader += strlen(reader) + 1;
	}

	//ask the user to select a reader
	for (int i = 0; i < readers.size(); i++) {
		std::cout << (i + 1) << ") " << readers[i] << std::endl;
	}
	std::cout << "Select a reader" << std::endl;
	int readerNum = -1;
	std::cin >> readerNum;
	if (readerNum < 1 || readerNum > readers.size()) {
		std::cerr << "Reader not found" << std::endl;
		exit(-3);
	}
	
	//obtain the handle to the specified reader
	NISHandle handle = NIS_GetHandle(readers[readerNum - 1]);
	if(handle == nullptr) {
		std::cerr << "Error obtaining reader handle" << std::endl;
		exit(-4);
	}

	//read the NIS
	char nisData[300];
	if(NIS_ReadNis(handle, nisData, sizeof(nisData), callback)) {
		std::cerr << "Could not read the NIS from token" << std::endl;
		exit(-5);
	}
	else {
		//wait while the callback read the NIS at regular interval
		for(int i = 0; i < 10; ++i)
			std::this_thread::sleep_for(std::chrono::milliseconds {1000});

		NIS_StopPoll(handle);
	}

	return 0;
}
