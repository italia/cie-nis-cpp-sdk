#include "nis.h"

#include <unordered_map>
#include <vector>
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
	unordered_map<BackendType,Reader*> backends;
	vector<string> readers;
	char *idList;
public:
	NISManager(const NISManager &) = delete;
	void operator=(const NISManager &) = delete;
	void addBackend(BackendType backtype, Reader *backend) { backends[backtype] = backend; }
	Reader* removeBackend(BackendType backtype) { 
		Reader* backend = nullptr; 
		auto it = backends.find(backtype);
		if(it != backends.end()) { 
			backend = it->second; 
			backends.erase(it); 
		} 
		return backend; 
	}
	const unordered_map<BackendType,Reader*> &getBackends() { return backends; };
	vector<string> &getReaders() { return readers; }
	char* getIdentifiersList() { return idList; }
	void deleteIdentifiersList() { if(idList) delete[] idList; }
	char* allocateIdentifiersList(size_t len) { deleteIdentifiersList(); idList = new char[len]; return idList; }
};

int NIS_Init(uint32_t backendBitfield)
{
	int ret = 0;

	if(backendBitfield & NIS_BACKEND_PCSC) {
		ReaderPCSC* backend = new ReaderPCSC();
		if(backend->hasContext())
			NISManager::getInstance().addBackend(NIS_BACKEND_PCSC, backend);
		else {
			delete backend;
			ret = -1;
		}
	}
	//else ... //add backends here
	
	return ret;
}

int NIS_ReaderList(char **readers, size_t *len)
{
	int ret = 0;

	vector<string> &allReaders = NISManager::getInstance().getReaders();
	allReaders.clear();

	for(auto &backend : NISManager::getInstance().getBackends())
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

NISHandle NIS_GetHandle(char *readerName)
{
	for(auto &backend : NISManager::getInstance().getBackends())
	{
		Token* token = backend.second->getToken(string(readerName));
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

int NIS_StopPoll(NISHandle handle)
{
	pollCntr.exitPoll = true;
	NIS_JoinThread(&pollCntr.th);
}

int NIS_ConfigHandle(NISHandle handle, uint32_t config)
{
	return 0;
}

int NIS_Deinit(uint32_t backendBitfield)
{
	if(backendBitfield & NIS_BACKEND_PCSC) {
		Reader *erased = NISManager::getInstance().removeBackend(NIS_BACKEND_PCSC);
		if(erased)
			delete erased;
	}
	//else ... //add backends here
	
	NISManager::getInstance().deleteIdentifiersList();

	return 0;
}

int NIS_Reset(NISHandle handle)
{
	return 0;
}
