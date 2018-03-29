#ifndef DIGEST_H_GUARD
#define DIGEST_H_GUARD

#include <crypto++/sha.h>
#include <stdint.h>

namespace cie 
{
	namespace nis 
	{
		namespace utils 
		{
			bool digest_SHA256(void* input, size_t length, uint8_t* md);
		}
	}
}

#endif
