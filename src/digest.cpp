#include "digest.h"

bool cie::nis::utils::digest_SHA256(void* data, size_t length, uint8_t* md)
{
#if false
	bool ret = false;
	bool hasInit = false;
	SHA256_CTX context;

	hasInit = SHA256_Init(&context);

	if(hasInit)
		ret = SHA256_Update(&context, input, length);

	if(hasInit)
	{
		bool hasFinish = SHA256_Final(md, &context);
		if(ret)
			ret = hasFinish;
	}

	return ret;
#else
	//TODO: check the return code
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	CryptoPP::SHA256().CalculateDigest(md, (byte*)data, length);

	return true;
#endif
}
