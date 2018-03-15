#include <iostream>
#include "thread.h"
#include "../../executor.h"

namespace nis 
{
	namespace helper
	{
		int NIS_CreateThread(NISThread &th, NISThFunc fnc, std::shared_ptr<PollExecutor> pe)
		{
			if(pthread_create(&th, NULL, fnc, pe.get())) {
				std::cerr << "Error creating thread" << std::endl;
				return -1;
			}

			return 0;
		}

		int NIS_JoinThread(NISThread &th)
		{
			if(pthread_join(th, NULL)) {
				std::cerr << "Error joining thread" << std::endl;
				return -1;
			}

			return 0;
		}
	}
}

