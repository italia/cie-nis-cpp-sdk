#ifndef  EXECUTOR_GUARD
#define  EXECUTOR_GUARD

#include <thread>
#include <iostream>
#include "nis_types.h"

struct PollExecutor {
	nis_callback_t callback;
#ifdef USE_EXT_THREAD
	NISThread th;
#else
	std::thread th;
#endif
	NISHandle pollHandle;
	bool exitPoll;
	char *pollData;
	size_t pollSize; 
	uint32_t interval_ms;
	uint32_t uid;
};

#endif