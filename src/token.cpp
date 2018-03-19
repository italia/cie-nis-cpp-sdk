#include <memory>
#include "token.h"
#include "executor.h"
#include "uid.h"
#include "nis_manager.h"

using namespace std;

cie::nis::Token::~Token() {} //pure virtual destructors must be defined

static void *pollNis(weak_ptr<PollExecutor> pe)
{
	if(!pe.expired()) {
		shared_ptr<PollExecutor> cntr = pe.lock();

		if(cntr) {
			while(!cntr->exitPoll) {
				if(!((cie::nis::Token*)cntr->pollHandle)->readNis(cntr->pollData, nullptr, 0, nullptr))
					cntr->callback(cntr->pollData, cntr->pollSize);
				std::this_thread::sleep_for(std::chrono::milliseconds {cntr->interval_ms});
			}
		}
	}
}

int cie::nis::Token::configure(uint32_t config)
{
	return 0;
}

int cie::nis::Token::reset()
{
	return 0;
}

int cie::nis::Token::readNis(char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid)
{
	int ret = 0;
	const size_t lenData = NIS_LENGTH;
	if(callback && uid) {
		*uid = Utils::getUid();
		shared_ptr<PollExecutor> pollCntr{new PollExecutor{}};

		pollCntr->uid = *uid;
		pollCntr->pollHandle = this;
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
		string nis = readNis();
		strcpy(nisData, nis.c_str());

		return 0;
	}

	return -1;
}

string cie::nis::Token::readNis()
{
	const size_t lenData = NIS_LENGTH;

	std::vector<BYTE> response(lenData);

	if(Requests::select_df_ias(*this, response)) {
		if(Requests::select_df_cie(*this, response)) {
			if(Requests::read_nis(*this, response)) {
				return string{(char*)response.data()};
			}
		}
	} 
}
