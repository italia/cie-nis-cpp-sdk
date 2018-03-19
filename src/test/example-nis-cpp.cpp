/** @file example-nis.cpp
 *  @brief Test example to read the NIS using the cie-nis-cpp-sdk library.
 * 
 *  This example initialize all implemented backends, enumerate the connected readers, let the user choose a reader and then try to read the NIS from the inserted token. The read is done via background polling and callback method. On exit, all initialized backends are freed.
 */

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include "nis.h"

#define _TCHAR char

using namespace std;

void deinit(void)
{
	//deinit all subsystems
	cie::nis::deinit(NIS_BACKEND_ALL);
}

void callback1(char *const nisData, size_t lenData)
{
	nisData[NIS_LENGTH] = '\0';
	std::cout << "NIS thA: " << std::string{nisData} << std::endl;
}

void callback2(char *const nisData, size_t lenData)
{
	nisData[NIS_LENGTH] = '\0';
	std::cout << "NIS thB: " << std::string{nisData} << std::endl;
}

int main(int argc, _TCHAR* argv[])
{
	//init all subsystems (PCSC, ...)
	if(cie::nis::init(NIS_BACKEND_ALL)) {
		std::cerr << "Error initializing backend sybsystem" << std::endl;
		exit(-1);
	}
	atexit(deinit);

	//obtain the flat list of all readers from all backends
	vector<string> readers = cie::nis::readersList();
	
	int readerNum = 1;
	//ask the user to select a reader, if more than one is present
	if(readers.size() > 1)
	{
		for (int i = 0; i < readers.size(); i++) {
			std::cout << (i + 1) << ") " << readers[i] << std::endl;
		}
		std::cout << "Select a reader" << std::endl;
		std::cin >> readerNum;
		if (readerNum < 1 || readerNum > readers.size()) {
			std::cerr << "Reader not found" << std::endl;
			exit(-3);
		}
	}
	
	//obtain the handle to the specified reader
	cie::nis::Token* token = cie::nis::getToken(readers[readerNum - 1]);
	if(token == nullptr) {
		std::cerr << "Error obtaining reader handle" << std::endl;
		exit(-4);
	}

	//read the NIS
	char nisData1[NIS_LENGTH+1];	//to take into account for the termination character
	char nisData2[NIS_LENGTH+1];	//to take into account for the termination character
	uint32_t uid1, uid2;
	if(token->readNis(nisData1, callback1, 1000/*ms*/, &uid1)) {
		std::cerr << "Could not start the 1st NIS reading thread" << std::endl;
		exit(-5);
	}
	if(token->readNis(nisData2, callback2, 1000/*ms*/, &uid2)) {
		std::cerr << "Could not start the 2nd NIS reading thread" << std::endl;
		exit(-6);
	}
		
	//wait while the callback read the NIS at regular interval
	for(int i = 0; i < 10; ++i)
			std::this_thread::sleep_for(std::chrono::milliseconds {1000});

	cie::nis::stopPoll(uid1);
	cie::nis::stopPoll(uid2);

	return 0;
}
