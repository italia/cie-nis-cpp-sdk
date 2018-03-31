#include "utilexception.h"
#include <stdio.h>

extern thread_local std::unique_ptr<CFuncCallInfoList> callQueue;

logged_error::logged_error(const char *message) : std::runtime_error(message) {
	OutputDebugString(what());
	CFuncCallInfoList *ptr = callQueue.get();
	while (ptr != nullptr) {
		OutputDebugString(ptr->info->FunctionName());
		ptr = ptr->next.get();
	}
}
