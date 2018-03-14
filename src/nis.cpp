/** @file nis.cpp
 *  @brief Main facilities
 *
 *  This file contains the most important core functions of the entire NIS sdk. Just include @e nis.h in your source code to use this API.
 */

#include "nis.h"

#include <unordered_map>
#include <vector>
#include <functional>
#include <cstring>
#include <algorithm>
#include "reader.h"
#include "reader_pcsc.h"
#include "token.h"
#include "token_pcsc.h"
#include "requests.h"
#include <thread>
#include "thread.h"

using namespace nis::interface;
using namespace nis::implementation;
using namespace nis::helper;

class NISManager
{
public:
	static NISManager& getInstance()
	{
		static NISManager instance;
		return instance;
	}
private:
	NISManager() {}
	unordered_map<BackendType,shared_ptr<Reader>,std::hash<int>> backends;
	vector<string> readers;
	char *idList;
public:
	NISManager(const NISManager &) = delete;
	void operator=(const NISManager &) = delete;
	void addBackend(BackendType backtype, shared_ptr<Reader> backend) { backends[backtype] = backend; }
	void removeBackend(BackendType backtype) { 
		auto it = backends.find(backtype);
		if(it != backends.end()) { 
			backends.erase(it); 
		} 
	}
	const unordered_map<BackendType,shared_ptr<Reader>,std::hash<int>> &getBackends() { return backends; };
	vector<string> &getReaders() { return readers; }
	char* getIdentifiersList() { return idList; }
	void deleteIdentifiersList() { if(idList) delete[] idList; }
	char* allocateIdentifiersList(size_t len) { deleteIdentifiersList(); idList = new char[len]; return idList; }
};

/** 
 * Initialize the NIS sdk backends.
 * @param[in] backendBitfield Bitfield representing the backends to be initialized, taken from ::BackendType
 * @see 
 * @return 0 on success, negative on error
 */
int NIS_Init(uint32_t backendBitfield)
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

/** 
 * Obtain the connected reader list.
 * @param[out] readers if the value @e len points to is 0 then @e readers will be automatically allocated (and automatically freed on deinit, so no user intervention is required). If not, readers must point to a pointer to a valid array allocated by the caller who is the owner of that array
 * @param[in,out] len could point to a value of 0 for auto-allocation, or to the size of @e readers array when it's provided by the user
 * @return 0 on success, negative on error. If negative, the abs value stands for the number of backends for which it was not possible to get the reader list
 */
int NIS_ReaderList(char **readers, size_t *len)
{
	int ret = 0;

	vector<string> &allReaders = NISManager::getInstance().getReaders();
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

	int totLen = 0;
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

	return ret;
}

/** 
 * Obtain the handler of a specific reader.
 * @param[in] readerName a string  representing the name of the reader. A list of readers name can be obtain invoking ::NIS_ReaderList()
 * @return 0 on success, negative on error.
 */
NISHandle NIS_GetHandle(char *readerName)
{
	for(auto backend : NISManager::getInstance().getBackends())
	{
		Token* token = backend.second->getToken(string(readerName)).get();
		if(token && token->connect() == TOK_RESULT_OK)
			return token;
	}

	return nullptr;
}

//temporary! What follows must be converted to thread-safe data and code
static struct PollContainer { 
	nis_callback_t callback;
	NISThread th;	
	bool exitPoll;
	NISHandle pollHandle;
	char *pollData;
	size_t pollSize;
} pollCntr;
static void *pollNis(void *data)
{
	PollContainer* cntr = (PollContainer*)data;

	while(!pollCntr.exitPoll) {
		if(!NIS_ReadNis(cntr->pollHandle, cntr->pollData, cntr->pollSize, nullptr)) 
			cntr->callback(cntr->pollData, cntr->pollSize);
		std::this_thread::sleep_for(std::chrono::milliseconds {1000});
	}
}
//----------------------------------------------------------

/** 
 * Read the NIS from the specified token.
 * @param[in] handle the handler of the reader previously obtained calling ::NIS_GetHandle()
 * @param[out] nisData array in which to store the NIS read back from the token
 * @param[in] lenData the size of the @e nisData array
 * @param[in] callback if @e NULL the call is blocking and the NIS is copied inside @e nisData upon return, otherwise the function spawns a background thread and returns immediately. The thread will invoke the callback funcion passing to it the read NIS @sa ::nis_callback_t
 * @return 0 on success, negative on error.
 */
int NIS_ReadNis(NISHandle handle, char *const nisData, size_t lenData, nis_callback_t callback)
{
	if(callback) {
		pollCntr.pollHandle = handle;
		pollCntr.callback = callback;
		pollCntr.exitPoll = false;
		pollCntr.pollData = nisData;
		pollCntr.pollSize = lenData;
		return NIS_CreateThread(&pollCntr.th, pollNis, &pollCntr);
	}
	else {
		std::vector<BYTE> response(RESPONSE_SIZE);

		if(Requests::select_df_ias(*((Token*)handle), response)) {
			if(Requests::select_df_cie(*((Token*)handle), response)) {
				if(Requests::read_nis(*((Token*)handle), response)) {
					memcpy(nisData, response.data(), min(response.size(), lenData));
					return 0;
				}
			}
		}
	}

	return -1;
}

/** 
 * Stop the polling on a specified reader started invoking ::NIS_ReadNis() with a callback function.
 * @param[in] handle the handler of the reader previously obtained calling ::NIS_GetHandle()
 * @return 0 on success, negative on error.
 */
int NIS_StopPoll(NISHandle handle)
{
	pollCntr.exitPoll = true;
	return NIS_JoinThread(&pollCntr.th);
}

int NIS_ConfigHandle(NISHandle handle, uint32_t config)
{
	return 0;
}

/** 
 * Deinitialize the NIS sdk backends. Frees all related data, so handlers obtained through ::NIS_GetHandle() or the reader list allocated by ::NIS_ReaderList() are not valid anymore.
 * @param[in] backendBitfield Bitfield representing the backends to be deinitialized, taken from ::BackendType
 * @see 
 * @return 0 on success, negative on error
 */
int NIS_Deinit(uint32_t backendBitfield)
{
	if(backendBitfield & NIS_BACKEND_PCSC) {
		NISManager::getInstance().removeBackend(NIS_BACKEND_PCSC);
	}
	//else ... //add backends here
	
	NISManager::getInstance().deleteIdentifiersList();

	return 0;
}

int NIS_Reset(NISHandle handle)
{
	return 0;
}
