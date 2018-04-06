#ifndef DIGEST_H_GUARD
#define DIGEST_H_GUARD

#include <crypto++/sha.h>
#include <stdint.h>
#include <util/array.h>

namespace cie 
{
	namespace nis 
	{
		namespace utils 
		{
			bool digest_SHA256(void* input, size_t length, uint8_t* md);
			ByteDynArray digest_SHA256(const ByteArray &data);
			ByteDynArray digest_SHA1(const ByteArray &data);
		}
	}
}

#endif
