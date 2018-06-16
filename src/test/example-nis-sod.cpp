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
	if(readers.empty()) {
		std::cerr << "No readers found" << std::endl;
		exit(-1);
	}

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
	std::cout << "NIS: " << token->readNis(AUTH_INTERNAL) << std::endl;

	return 0;
}
