/** @file nis_manager.cpp
 *  @brief NIS Manager implementation
 *
 *  This file contains the implementation of teh NIS Manager.
 */

#include "nis_manager.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <thread>
#include "reader_pcsc.h"
#include "requests.h"
#include "token.h"
#include "token_pcsc.h"
#include "uid.h"

NISManager &NISManager::getInstance() {
  static NISManager instance;
  return instance;
}

vector<string> &NISManager::getReaders() { return readers; }

const NISManager::BackendsMap &NISManager::getBackends() { return backends; };

void NISManager::addBackend(BackendType backtype, shared_ptr<Reader> backend) {
  backends[backtype] = backend;
}

void NISManager::removeBackend(BackendType backtype) {
  auto it = backends.find(backtype);
  if (it != backends.end()) {
    backends.erase(it);
  }
}

void NISManager::addExecutor(uint32_t uid, shared_ptr<PollExecutor> ex) {
  executors[uid] = ex;
}

shared_ptr<PollExecutor> NISManager::removeExecutor(uint32_t uid,
                                                    bool mustLock) {
  auto optionalLock =
      mustLock ? unique_lock<mutex>{execMutex} : unique_lock<mutex>{};

  auto it = executors.find(uid);
  if (it == executors.end()) return nullptr;

  executors.erase(it);
  shared_ptr<PollExecutor> pe = it->second;
  pe->exitPoll = true;
#ifdef USE_EXT_THREAD
  NIS_JoinThread(pe->th);
#else
  pe->th.join();
#endif
  return pe;
}

void NISManager::removeAllExecutors() {
  vector<uint32_t> keys;
  lock_guard<mutex> lock(execMutex);
  for (auto it : executors) keys.push_back(it.first);
  for (auto it : keys) removeExecutor(it, false);
}

void NISManager::lockExecutors() { execMutex.lock(); }

void NISManager::unlockExecutors() { execMutex.unlock(); }

char *NISManager::getIdentifiersList() { return idList; }

void NISManager::deleteIdentifiersList() {
  if (idList) delete[] idList;
}

char *NISManager::allocateIdentifiersList(size_t len) {
  deleteIdentifiersList();
  idList = new char[len];
  return idList;
}
