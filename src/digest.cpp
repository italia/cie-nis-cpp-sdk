#include "digest.h"

bool cie::nis::utils::digest_SHA256(void* data, size_t length, uint8_t* md)
{
	//TODO: check the return code
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	CryptoPP::SHA256().CalculateDigest(md, (byte*)data, length);

	return true;
}

ByteDynArray cie::nis::utils::digest_SHA256(const ByteArray &data)
{
	ByteDynArray md(CryptoPP::SHA256::DIGESTSIZE);

	//TODO: check the return code
	CryptoPP::SHA256().CalculateDigest(md.data(), (byte*)data.data(), data.size());

	return md;
}

ByteDynArray cie::nis::utils::digest_SHA1(const ByteArray &data)
{
	ByteDynArray md(CryptoPP::SHA1::DIGESTSIZE);

	//TODO: check the return code
	CryptoPP::SHA1().CalculateDigest(md.data(), (byte*)data.data(), data.size());

	return md;
}
