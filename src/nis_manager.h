#ifndef NISMANAGER_GUARD
#define NISMANAGER_GUARD

/** @file nis_manager.h
 *  @brief NIS Manager
 *
 *  This file contains the declaration of teh NIS Manager, a singleton that
 * provide facilities to manage backends, tokens and associated executors.
 */

#include "executor.h"
#include "nis.h"
#include "nis_types.h"
#include "reader.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace cie::nis;
using namespace std;

/**
 * Singleton that manages backends list, tokens and associated executors.
 */
class NISManager {
 public:
  using BackendsMap =
      unordered_map<BackendType, shared_ptr<Reader>, std::hash<int>>;

  using ExecutorsMap = unordered_map<uint32_t, shared_ptr<PollExecutor>>;

  /**
   * Singleton factory method. Call this to obtain the unique reference to the
   * NIS Manager.
   */
  static NISManager &getInstance();

 private:
  NISManager() = default;

  /** List all backends currently available after having initialized the
   * relative backend calling ::NIS_Init */
  BackendsMap backends;

  /** List all executors (that should be running) */
  ExecutorsMap executors;

  /** Internal representation of the array of readers, this is the list of
   * readers' name */
  vector<string> readers;

  /** Multi-string representation of the list of reader names. This reflect the
   * pcsc-lite's way to represent the list of available readers */
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
  const BackendsMap &getBackends();

  /**
   * Add a backend to the backends list
   * @param[in] backendType Represents the backend to be added, taken from
   * ::BackendType
   * @param[in] backend pointer to the backend to be added
   * @see getBackends()
   * @see removeBackend()
   */
  void addBackend(BackendType backtype, shared_ptr<Reader> backend);

  /**
   * Remove a backend from the backend list
   * @param[in] backendType Represents the backend to be removed, taken from
   * ::BackendType
   * @see getBackends()
   * @see addBackend()
   */
  void removeBackend(BackendType backtype);

  /**
   * Get the list of the names of all readers connected
   * @return the array of the reader names
   */
  vector<string> &getReaders();

  /**
   * Add a ::PollExecutor to the executors list
   * @param[in] uid a unique identifier that serves as an handler to represent
   * this executor in future calls
   * @param[in] ex pointer to the ::PollExecutor to be added
   * @see removeExecutor()
   * @see removeAllExecutors()
   * @see lockExecutors()
   * @see unlockExecutors()
   */
  void addExecutor(uint32_t uid, shared_ptr<PollExecutor> ex);
  shared_ptr<PollExecutor> removeExecutor(uint32_t uid, bool mustLock = true);
  void removeAllExecutors();
  void lockExecutors();
  void unlockExecutors();

  char *getIdentifiersList();
  void deleteIdentifiersList();
  char *allocateIdentifiersList(size_t len);
};

#endif  // NISMANAGER_GUARD
