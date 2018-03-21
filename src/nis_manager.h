#ifndef NISMANAGER_GUARD
#define NISMANAGER_GUARD

/** @file nis_manager.h
 *  @brief NIS Manager
 *
 *  This file contains the declaration of teh NIS Manager, a singleton that provide facilities to manage backends, tokens and associated executors.
 */

#include "nis.h"

#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <cstring>
#include <algorithm>
#include "reader.h"
#include "reader_pcsc.h"
#include "token.h"
#include "token_pcsc.h"
#include "requests.h"
#include "executor.h"
#include "uid.h"

using namespace cie::nis;

/**
 * Singleton that manages backends list, tokens and associated executors.
 */
class NISManager
{
public:
	/**
	 * Singleton factory method. Call this to obtain the unique reference to the NIS Manager.
	 */
	static NISManager& getInstance()
	{
		static NISManager instance;
		return instance;
	}
private:
	NISManager() {}
	/** List all backends currently available after having initialized the relative backend calling ::NIS_Init */
	unordered_map<BackendType,shared_ptr<Reader>,std::hash<int>> backends;
	/** List all executors (that should be running) */
	unordered_map<uint32_t,shared_ptr<PollExecutor>> executors;
	/** Internal representation of the array of readers, this is the list of readers' name */
	vector<string> readers;
	/** Multi-string representation of the list of reader names. This reflect the pcsc-lite's way to represent the list of available readers */
	char *idList;
	/** Mutex that allows operations on executors list to be thread-safe */
	mutex execMutex;
public:
	/** We really don't want to let this singleton being copied */
	NISManager(const NISManager &) = delete;
	void operator=(const NISManager &) = delete;

	/**
	 * Obtain a list of all backends currently initialized succesfully
	 * @return a map whose values points to a specific backend
	 * @see addBackend()
	 * @see removeBackend()
	 */
	const unordered_map<BackendType,shared_ptr<Reader>,std::hash<int>> &getBackends() { return backends; };
	/**
	 * Add a backend to the backends list
	 * @param[in] backendType Represents the backend to be added, taken from ::BackendType
	 * @param[in] backend pointer to the backend to be added
	 * @see getBackends()
	 * @see removeBackend()
	 */
	void addBackend(BackendType backtype, shared_ptr<Reader> backend) { backends[backtype] = backend; }
	/**
	 * Remove a backend from the backend list
	 * @param[in] backendType Represents the backend to be removed, taken from ::BackendType
	 * @see getBackends()
	 * @see addBackend()
	 */
	void removeBackend(BackendType backtype) { 
		auto it = backends.find(backtype);
		if(it != backends.end()) { 
			backends.erase(it); 
		} 
	}

	/**
	 * Get the list of the names of all readers connected
	 * @return the array of the reader names
	 */
	vector<string> &getReaders() { return readers; }

	/**
	 * Add a ::PollExecutor to the executors list
	 * @param[in] uid a unique identifier that serves as an handler to represent this executor in future calls
	 * @param[in] ex pointer to the ::PollExecutor to be added
	 * @see removeExecutor()
	 * @see removeAllExecutors()
	 * @see lockExecutors()
	 * @see unlockExecutors()
	 */
	void addExecutor(uint32_t uid, shared_ptr<PollExecutor> ex) { executors[uid] = ex; }
	shared_ptr<PollExecutor> removeExecutor(uint32_t uid, bool mustLock=true);
	void removeAllExecutors() { vector<uint32_t> keys; lock_guard<mutex> lock{execMutex}; for(auto it : executors) keys.push_back(it.first); for(auto it : keys) removeExecutor(it, false); }
	void lockExecutors() { execMutex.lock(); }
	void unlockExecutors() { execMutex.unlock(); }

	char* getIdentifiersList() { return idList; }
	void deleteIdentifiersList() { if(idList) delete[] idList; }
	char* allocateIdentifiersList(size_t len) { deleteIdentifiersList(); idList = new char[len]; return idList; }
};

#endif
