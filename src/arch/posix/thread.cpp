#include <iostream>
#include "thread.h"

namespace nis 
{
	namespace helper
	{
		int NIS_CreateThread(NISThread* th, NISThFunc fnc, void *data)
		{
			if(pthread_create(th, NULL, fnc, data)) {
				std::cerr << "Error creating thread" << std::endl;
				return -1;
			}

			return 0;
		}

		int NIS_JoinThread(NISThread* th)
		{
			if(pthread_join(*th, NULL)) {
				std::cerr << "Error joining thread" << std::endl;
				return -1;
			}

			return 0;
		}
	}
}

