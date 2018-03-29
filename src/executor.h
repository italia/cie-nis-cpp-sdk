/** @file executor.h
 * @brief Executor thread
 *
 * An executor allows to perform deferred work on a token
 */

#ifndef  EXECUTOR_GUARD
#define  EXECUTOR_GUARD

#include <thread>
#include <iostream>
#include "nis_types.h"

/** PollExecutor performs some specific work (in polling) and the call a callback passing the results, at regular intervals */
struct PollExecutor {
	/** the callback to be called each interval has expired */
	nis_callback_t callback;
	/** internal thread handler that manages the deferred work and, after that, calls the callback  with the result of that work */
#ifdef USE_EXT_THREAD
	NISThread th;
#else
	std::thread th;
#endif
	/** handle of the token you're performing the callback on */
	NISHandle pollHandle;
	/** flag to indicate the thread has to stop. Setting this will let the deferred activity to end gracefully */
	bool exitPoll;
	/** point to the data associated to this executor. Will be passed as a parameter to the callback */
	char *pollData;
	/** size of the ::pollData array. Will be passed as a parameter to teh callback */
	size_t pollSize; 
	/** interval in milliseconds between polls */
	uint32_t interval_ms;
	/** unique identifier that represents this executor */
	uint32_t uid;
	/** method of authetication, see ::AuthType */
	AuthType auth;
};

#endif
