#include "thread.h"
#include <pthread.h>
#include <iostream>

namespace cie {
namespace nis {
namespace helper {

int NIS_CreateThread(NISThread &th, NISThFunc fnc,
                     std::weak_ptr<PollExecutor> pe) {
  if (pe.expired()) return -2;

  std::shared_ptr<PollExecutor> cntr{pe};
  if (pthread_create(&th, NULL, fnc, pe.lock().get())) {
    std::cerr << "Error creating thread" << std::endl;
    return -1;
  }

  return 0;
}

int NIS_JoinThread(NISThread &th) {
  if (pthread_join(th, NULL)) {
    std::cerr << "Error joining thread" << std::endl;
    return -1;
  }

  return 0;
}

}  // namespace helper
}  // namespace nis
}  // namespace cie
