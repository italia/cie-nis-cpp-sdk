#ifndef THREAD_GUARD
#define THREAD_GUARD

#include <memory>
#include <pthread.h> 
#include "../../executor.h"

namespace cie {
namespace nis {
namespace helper {

typedef pthread_t NISThread;
typedef void *(*NISThFunc)(void *);

int NIS_CreateThread(NISThread &th, NISThFunc fnc, std::weak_ptr<PollExecutor>);
int NIS_JoinThread(NISThread &th);

}  // namespace helper
}  // namespace nid
}  // namespace cie

#endif  // THREAD_GUARD
