#include "sod.h"

bool cie::nis::sodParser(cie_BerTriple *triple, void *data) 
{
	static byte oid_mRTDSignatureData[] = {0x67, 0x81, 0x08, 0x01, 0x01, 0x01}; //2.23.136.1.1.1
	static byte oid_SHA256[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01}; //2.16.840.1.101.3.4.2.1
	static SodFsm sodFsm = SodFsm::BEGIN;
	Sod *sod = (Sod*)data;

	switch(sodFsm)
	{
		case SodFsm::BEGIN:
			if(triple->type == 0x06/*OID*/)
				if(triple->contentLength == sizeof(oid_mRTDSignatureData) && !memcmp(triple->value, oid_mRTDSignatureData, triple->contentLength))
					sodFsm = SodFsm::SIGNED_DATA;
			break;
		case SodFsm::SIGNED_DATA:
			if(triple->type == 0x06/*OID*/)
				if(triple->contentLength == sizeof(oid_SHA256) && !memcmp(triple->value, oid_SHA256, triple->contentLength))
				{
					sod->signedDataHash = HashType::SHA256;
					sodFsm = SodFsm::HASH_TYPES;
				}
			break;
		case SodFsm::HASH_TYPES:
			if(triple->type == 0x02/*INTEGER*/)
				if(triple->value[0] == 0xA1)	//EF.idService
				{
					sodFsm = SodFsm::IDSERVICE_HASH;
				}
			break;
		case SodFsm::IDSERVICE_HASH:
			if(triple->type == 0x04/*OCTET STRING*/)
				if(triple->contentLength == sizeof(sod->idServiceHash))
				{
					memcpy(sod->idServiceHash, triple->value, triple->contentLength);
					sodFsm = SodFsm::HASH_TYPES;
				}
			break;
	}

	return true;
}

