#ifndef THREAD_GUARD
#define THREAD_GUARD

#include <pthread.h> 

namespace nis {
	namespace helper {
		typedef pthread_t NISThread; 
		typedef void*(*NISThFunc)(void*);

		int NIS_CreateThread(NISThread* th, NISThFunc fnc, void *data);
		int NIS_JoinThread(NISThread* th);
	}
}

#endif
