/** @file nis.cpp
 *  @brief Main facilities
 *
 *  This file contains the most important core functions of the entire NIS sdk. Just include @e nis.h in your source code to use this API.
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
#include "thread.h"
#include "uid.h"
#include "nis_manager.h"

using namespace cie::nis;
using namespace cie::nis::helper;
using namespace std;

namespace cie {
	namespace nis {
		  int init(uint32_t backendBitfield)
		  {	
			int ret = 0;

			if(backendBitfield & NIS_BACKEND_PCSC) {
				shared_ptr<ReaderPCSC> backend{new ReaderPCSC()};
				if(backend->hasContext())
					NISManager::getInstance().addBackend(NIS_BACKEND_PCSC, backend);
				else {
					ret = -1;
				}
			}
			//else ... //add backends here
				
			return ret;
		  }

		  vector<string> readersList()
		  {	
			int ret = 0;

			vector<string> allReaders = NISManager::getInstance().getReaders();
			allReaders.clear();

			for(auto backend : NISManager::getInstance().getBackends())
			{
				if(backend.second->enumerateReaderList() == READER_RESULT_OK)
				{
					vector<string> v = backend.second->getReaderList();
					allReaders.insert(allReaders.end(), v.begin(), v.end());
				}
				else --ret;
			}

			return allReaders;
		  }

		  Token* getToken(string readerName)
		  {
			for(auto backend : NISManager::getInstance().getBackends())
			{
				Token* token = backend.second->getToken(string(readerName)).get();
				if(token && token->connect() == TOK_RESULT_OK)
					return token;
			}

			return nullptr;
		  }

		  int deinit(uint32_t backendBitfield)
		  {
			NISManager::getInstance().removeAllExecutors();

			if(backendBitfield & NIS_BACKEND_PCSC) {
				NISManager::getInstance().removeBackend(NIS_BACKEND_PCSC);
			}
			//else ... //add backends here
			
			NISManager::getInstance().deleteIdentifiersList();

			return 0;
		  }

		  int stopPoll(uint32_t uid)
		  {
			shared_ptr<PollExecutor> pe = NISManager::getInstance().removeExecutor(uid);
			return 0;
		  }
	}
}
/** 
 * Initialize the NIS sdk backends.
 * @param[in] backendBitfield Bitfield representing the backends to be initialized, taken from ::BackendType
 * @see 
 * @return 0 on success, negative on error
 */
int NIS_Init(uint32_t backendBitfield)
{
	return cie::nis::init(backendBitfield);
}

/** 
 * Obtain the connected reader list.
 * @param[out] readers if the value @e len points to is 0 then @e readers will be automatically allocated (and automatically freed on deinit, so no user intervention is required). If not, readers must point to a pointer to a valid array allocated by the caller who is the owner of that array
 * @param[in,out] len could point to a value of 0 for auto-allocation, or to the size of @e readers array when it's provided by the user
 * @return 0 on success, negative on error. If negative, the abs value stands for the number of backends for which it was not possible to get the reader list
 */
int NIS_ReaderList(char **readers, size_t *len)
{
	int totLen = 0;
	vector<string> allReaders = cie::nis::readersList();

	for(auto readerName : allReaders)
		totLen += readerName.length();
	totLen += allReaders.size() + 1;

	char *str;
	if(!*len)
	{
		str = NISManager::getInstance().allocateIdentifiersList(totLen);
		*len = totLen;	
		*readers = str;
	}
	else str = *readers;

	int remainingLen = *len;
	char *s = str;
	for(auto readerName : allReaders)
	{
		strncpy(s, readerName.c_str(), remainingLen);
		remainingLen -= readerName.length() + 1;
		s += readerName.length() + 1; 
	}
	if(remainingLen > 0)
		*s = '\0';

	return 0;
}

/** 
 * Obtain the handler of a specific reader.
 * @param[in] readerName a string  representing the name of the reader. A list of readers name can be obtain invoking ::NIS_ReaderList()
 * @return 0 on success, negative on error.
 */
NISHandle NIS_GetHandle(char *readerName)
{
	return cie::nis::getToken(string{readerName});
}

/** 
 * Read the NIS from the specified token.
 * @param[in] handle the handler of the reader previously obtained calling ::NIS_GetHandle()
 * @param[out] nisData array in which to store the NIS read back from the token. Should be long enough to contains the entire nis as a C-style sring, i.e. 12 (NIS) + 1 (null terminator) = 13 chars
 * @param[in] callback if @e NULL the call is blocking and the NIS is copied inside @e nisData upon return, otherwise the function spawns a background thread and returns immediately. The thread will invoke the callback funcion passing to it the read NIS @sa ::nis_callback_t
 * @param[in] interval time in ms between polls
 * @param[out] the UID associated to the newly created context of execution @sa ::NIS_StopPoll()
 * @return 0 on success, negative on error
 */
int NIS_ReadNis(NISHandle handle, char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid)
{
	/*int ret = 0;
	const size_t lenData = NIS_LENGTH;
	if(callback && uid) {
		*uid = Utils::getUid();
		shared_ptr<PollExecutor> pollCntr{new PollExecutor{}};

		pollCntr->uid = *uid;
		pollCntr->pollHandle = handle;
		pollCntr->callback = callback;
		pollCntr->exitPoll = false;
		pollCntr->pollData = nisData;
		pollCntr->pollSize = lenData;
		pollCntr->interval_ms = interval;

		NISManager::getInstance().lockExecutors();
#ifdef USE_EXT_THREAD
		ret = NIS_CreateThread(pollCntr.th, pollNis, weak_ptr<PollExecutor>{pollCntr});
#else
		try {
			pollCntr->th = thread(pollNis, weak_ptr<PollExecutor>{pollCntr});
		} catch(...) {
			cerr << "Cannot create polling thread." << endl;
		}
#endif
		NISManager::getInstance().addExecutor(*uid, pollCntr);

		NISManager::getInstance().unlockExecutors();

		return ret;
	} else {
		std::vector<BYTE> response(lenData);

		if(Requests::select_df_ias(*((Token*)handle), response)) {
			if(Requests::select_df_cie(*((Token*)handle), response)) {
				if(Requests::read_nis(*((Token*)handle), response)) {
					memcpy(nisData, response.data(), min(response.size(), lenData));
					return 0;
				}
			}
		}
	}

	return -1;*/

	return ((Token*)handle)->readNis(nisData, callback, interval, uid);
}

/** 
 * Stop the polling on a specified reader started invoking ::NIS_ReadNis() with a callback function.
 * @param[in] uid the UID of the context of execution previously obtained from e.g. ::NIS_ReadNis()
 * @return 0 on success, negative on error.
 */
int NIS_StopPoll(uint32_t uid)
{
	return cie::nis::stopPoll(uid);
}

int NIS_ConfigHandle(NISHandle handle, uint32_t config)
{
	return ((Token*)handle)->configure(config);
}

/** 
 * Deinitialize the NIS sdk backends. Frees all related data, so handlers obtained through ::NIS_GetHandle() or the reader list allocated by ::NIS_ReaderList() are not valid anymore.
 * @param[in] backendBitfield Bitfield representing the backends to be deinitialized, taken from ::BackendType
 * @see 
 * @return 0 on success, negative on error
 */
int NIS_Deinit(uint32_t backendBitfield)
{
	return cie::nis::deinit(backendBitfield);
}

int NIS_Reset(NISHandle handle)
{
	return ((Token*)handle)->reset();
}
