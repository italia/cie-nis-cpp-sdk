#include "exception.h"
#include <stdio.h>
#include <stdarg.h>

//extern thread_local std::unique_ptr<CFuncCallInfoList> callQueue;

logged_error::logged_error(const char *message) : std::runtime_error(message) {
	/*OutputDebugString(what());
	CFuncCallInfoList *ptr = callQueue.get();
	while (ptr != nullptr) {
		OutputDebugString(ptr->info->FunctionName());
		ptr = ptr->next.get();
	}*/
}

std::string stdPrintf(const char *format, ...) {
	std::string result;
	va_list args;
	va_start(args, format);
	int size = vprintf(format, args) + 1;
	result.resize(size);
	vsnprintf(&result[0], size, format, args);
	va_end(args);
	return result;
}

