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
  lock_guard<mutex> lock(execMutex);
  executors[uid] = ex;
}

namespace {

void removeSingleExecutor(PollExecutor *pe) {
  pe->exitPoll = true;
#ifdef USE_EXT_THREAD
  NIS_JoinThread(pe->th);
#else
  pe->th.join();
#endif
}

}  // namespace

shared_ptr<PollExecutor> NISManager::removeExecutor(uint32_t uid,
                                                    bool mustLock) {
  auto optionalLock =
      mustLock ? unique_lock<mutex>{execMutex} : unique_lock<mutex>{};

  auto it = executors.find(uid);
  if (it == executors.end()) return nullptr;

  auto pe = it->second;
  removeSingleExecutor(pe.get());
  executors.erase(it);
  return pe;
}

void NISManager::removeAllExecutors() {
  lock_guard<mutex> lock(execMutex);
  for (auto it : executors) removeSingleExecutor(it.second.get());
  executors.clear();
}

void NISManager::lockExecutors() { execMutex.lock(); }

void NISManager::unlockExecutors() { execMutex.unlock(); }

char *NISManager::getIdentifiersList() { return idList.data(); }

void NISManager::deleteIdentifiersList() {
  idList.clear();
}

char *NISManager::allocateIdentifiersList(size_t len) {
  idList.clear();
  idList.reserve(len);
  return idList.data();
}
