/** @file nis_manager.cpp
 *  @brief NIS Manager implementation
 *
 *  This file contains the implementation of teh NIS Manager.
 */

#include "nis_manager.h"

shared_ptr<PollExecutor> NISManager::removeExecutor(uint32_t uid, bool mustLock) { 
	if(mustLock)
		lock_guard<mutex> lock{execMutex}; 
	
	auto it = executors.find(uid); 
	if(it != executors.end()) 
	{ 
		executors.erase(it); 
		it->second->exitPoll = true;
		shared_ptr<PollExecutor> pe = it->second;
#ifdef USE_EXT_THREAD
		NIS_JoinThread(pe->th);
#else
		pe->th.join();
#endif
		return pe; 
	} 
	else return nullptr; 
}
