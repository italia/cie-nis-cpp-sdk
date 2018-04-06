#include <memory>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include "token.h"
#include "executor.h"
#include "uid.h"
#include "nis_manager.h"
#include "digest.h"
#include "ber/cie_BerReader.h"
#include "asn/ASNParser.h"

using namespace std;

cie::nis::Token::~Token() {} //pure virtual destructors must be defined

static void *pollNis(weak_ptr<PollExecutor> pe)
{
	if(!pe.expired()) {
		shared_ptr<PollExecutor> cntr = pe.lock();

		if(cntr) {
			while(!cntr->exitPoll) {
				if(!((cie::nis::Token*)cntr->pollHandle)->readNis(cntr->pollData, nullptr, 0, nullptr, cntr->auth))
					cntr->callback(cntr->pollData, cntr->pollSize);
				std::this_thread::sleep_for(std::chrono::milliseconds {cntr->interval_ms});
			}
		}
	}
}

int cie::nis::Token::configure(uint32_t config)
{
	return 0;
}

int cie::nis::Token::reset()
{
	return 0;
}

/*
int cie::nis::Token::parseSod(cieBerTripleCallbackFunc callback, Sod *sod)
{
	unsigned int lenData = 4096;

	std::vector<BYTE> response(lenData);

	if(Requests::select_df_ias(*this, response)) {
		if(Requests::select_df_cie(*this, response)) {
			cie_BerReader ber{this, sod};
			return ber.readTriples(0x06, callback, &lenData, 30) ? 0 : -1;
		}
	}

	return -2;	
}
*/

/** 
 * Verify if the SOD fiel is authentic and the elementary files have the correct digest
 * @return true if valid, false otherwise. Throws and exception in case of error.
 */
bool cie::nis::Token::verifySod(ByteArray &SOD, std::map<BYTE, ByteDynArray> &hashSet)
{
	CASNParser parser;
	parser.Parse(SOD);

	CASNTag &SODTag = *parser.tags[0];

	CASNTag &temp = SODTag.Child(0, 0x30);
	uint8_t OID[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02 };
	temp.Child(0, 06).Verify(VarToByteArray(OID));
	uint8_t val3=3;
	CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);
	temp2.Child(0, 2).Verify(VarToByteArray(val3));

	uint8_t OID_SH256[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01 };
	temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).Verify(VarToByteArray(OID_SH256));

	uint8_t OID3[] = { 0x67, 0x81, 0x08, 0x01, 0x01, 0x01 };
	temp2.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID3));
	ByteArray ttData = temp2.Child(2, 0x30).Child(1, 0xA0).Child(0, 04).content;
	CASNParser ttParser;
	ttParser.Parse(ttData);
	CASNTag &signedData = *ttParser.tags[0];
	signedData.CheckTag(0x30);

	CASNTag &signerCert = temp2.Child(3, 0xA0).Child(0, 0x30);
	CASNTag &temp3 = temp2.Child(4, 0x31).Child(0, 0x30);
	uint8_t val1 = 1;
	temp3.Child(0, 02).Verify(VarToByteArray(val1));
	CASNTag &issuerName = temp3.Child(1, 0x30).Child(0, 0x30);
	CASNTag &signerCertSerialNumber = temp3.Child(1, 0x30).Child(1, 02);
	temp3.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SH256));

	CASNTag &signerInfo = temp3.Child(3, 0xA0);
	uint8_t OID4[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x03 };
	signerInfo.Child(0, 0x30).Child(0, 06).Verify(VarToByteArray(OID4));
	uint8_t OID5[] = { 0x67, 0x81, 0x08, 0x01, 0x01, 0x01 };
	signerInfo.Child(0, 0x30).Child(1, 0x31).Child(0, 06).Verify(VarToByteArray(OID5));
	uint8_t OID6[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x04 };
	signerInfo.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID6));
	CASNTag &digest = temp3.Child(3, 0xA0).Child(1, 0x30).Child(1, 0x31).Child(0, 04);

	uint8_t OID_RSAwithSHA256[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0b };
	uint8_t OID_RSAwithSHA1[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05 };
	auto &digestAlgo = temp3.Child(4, 0x30).Child(0, 06).content;
	bool isSHA1 = false;
	bool isSHA256 = false;
	if (digestAlgo == VarToByteArray(OID_RSAwithSHA1))
		isSHA1 = true;
	else if (digestAlgo == VarToByteArray(OID_RSAwithSHA256))
		isSHA256 = true;
	else throw logged_error("Algoritmo del digest della firma non valido");

	CASNTag &signature = temp3.Child(5, 04);
	
	ByteDynArray calcDigest = cie::nis::utils::digest_SHA256(ttData.mid((int)signedData.startPos, (int)(signedData.endPos - signedData.startPos)));
	if (calcDigest!=digest.content)
		throw logged_error("Digest del SOD non corrispondente ai dati");

	ByteArray certRaw = SOD.mid((int)signerCert.startPos, (int)(signerCert.endPos - signerCert.startPos));
	const unsigned char* p = certRaw.data();
	unique_ptr<X509,std::function<void(X509*)>> certDS{d2i_X509(nullptr, &p, certRaw.size()), [](X509* val){ X509_free(val);}};
	if (certDS.get() == nullptr)
		throw logged_error("Certificato DS non valido");


	/*ByteArray pubKeyData(certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData, certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData);
	CASNParser pubKeyParser;
	pubKeyParser.Parse(pubKeyData);
	CASNTag &pubKey = *pubKeyParser.tags[0];
	CASNTag &modTag = pubKey.Child(0, 02);
	ByteArray mod = modTag.content;
	while (mod[0] == 0)
		mod = mod.mid(1);
	CASNTag &expTag = pubKey.Child(1, 02);
	ByteArray exp = expTag.content;
	while (exp[0] == 0)
		exp = exp.mid(1);

	ByteArray signatureData = signature.content;

	CRSA rsa(mod, exp);

	ByteDynArray decryptedSignature = rsa.RSA_PURE(signatureData);
	decryptedSignature = decryptedSignature.mid(RemovePaddingBT1(decryptedSignature));
	ByteArray toSign = SOD.mid((int)signerInfo.tags[0]->startPos, (int)(signerInfo.tags[signerInfo.tags.size()- 1]->endPos - signerInfo.tags[0]->startPos));
	ByteDynArray digestSignature;
	if (isSHA1) {
		CSHA1 sha1;
		decryptedSignature = decryptedSignature.mid(RemoveSha1(decryptedSignature));
		digestSignature = sha1.Digest(toSign.getASN1Tag(0x31));
	}
	if (isSHA256) {
		CSHA256 sha256;
		decryptedSignature = decryptedSignature.mid(RemoveSha256(decryptedSignature));
		digestSignature = sha256.Digest(toSign.getASN1Tag(0x31));
	}
	if (digestSignature!=decryptedSignature)
		throw logged_error("Firma del SOD non valida");
	*/

	ByteArray signatureData = signature.content;
	ByteArray toSign = SOD.mid((int)signerInfo.tags[0]->startPos, (int)(signerInfo.tags[signerInfo.tags.size()- 1]->endPos - signerInfo.tags[0]->startPos));
	ByteDynArray digestSignature;
	if (isSHA1) {
		digestSignature = cie::nis::utils::digest_SHA1(toSign.getASN1Tag(0x31));	//TODO: implement SHA1
	}
	if (isSHA256) {
		digestSignature = cie::nis::utils::digest_SHA256(toSign.getASN1Tag(0x31));
	}
	unique_ptr<EVP_PKEY,std::function<void(EVP_PKEY*)>> pkey{X509_get_pubkey(certDS.get()), [](EVP_PKEY* val){ EVP_PKEY_free(val);}};
	if(pkey.get() == nullptr)
		throw logged_error("Impossibile estrarre la public key dal certificato");
	RSA *rsa_pubkey = pkey.get()->pkey.rsa;//EVP_PKEY_get_RSA(pkey);
	if(rsa_pubkey == nullptr)
		throw logged_error("Public key non trovata");
	if(RSA_verify(isSHA256 ? NID_sha256 : NID_sha1, digestSignature.data(), digestSignature.size(), signatureData.data(), signatureData.size(), rsa_pubkey) != 1) {
		unsigned long err = ERR_get_error();
		throw logged_error("Errore durante la verifica RSA della firma");
	}

	//Verifica issuer
	issuerName.Reparse();
	X509_NAME *issuer = X509_get_issuer_name(certDS.get());
	size_t tagNo = X509_NAME_entry_count(issuer);
	if (issuerName.tags.size() != tagNo)
		throw logged_error("Issuer name non corrispondente");
	for (std::size_t i = 0; i < tagNo; ++i) {
		X509_NAME_ENTRY *e = X509_NAME_get_entry(issuer, i);
		ASN1_OBJECT *o = X509_NAME_ENTRY_get_object(e);
		ASN1_STRING *d = X509_NAME_ENTRY_get_data(e);
		CASNTag &SODElem = *issuerName.tags[i]->tags[0];
		uint8_t oidBuf[o->length];
		memcpy(oidBuf, o->data, sizeof(oidBuf));
		SODElem.tags[0]->Verify(ByteArray(oidBuf, sizeof(oidBuf)));
		SODElem.tags[1]->Verify(ByteArray(d->data, d->length));
	}

	ASN1_INTEGER *serial = X509_get_serialNumber(certDS.get());
	ByteDynArray certSerial = ByteArray(serial->data, serial->length);
	if (certSerial/*.reverse()*/ != signerCertSerialNumber.content)
		throw logged_error("Serial Number del certificato non corrispondente");

	// ora verifico gli hash dei DG
	uint8_t val0=0;
	signedData.Child(0, 02).Verify(VarToByteArray(val0));
	signedData.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SH256));

	std::map<BYTE, ByteDynArray> hashSod;
	CASNTag &hashTag = signedData.Child(2, 0x30);
	for (std::size_t i = 0; i<hashTag.tags.size();i++) {
		CASNTag &hashDG = *(hashTag.tags[i]);
		CASNTag &dgNum = hashDG.CheckTag(0x30).Child(0, 02);
		CASNTag &dgHash = hashDG.Child(1, 04);
		BYTE num = ByteArrayToVar(dgNum.content, BYTE);

		hashSod[num] = dgHash.content;
	}

	for (auto const &a : hashSet) {
		BYTE num = a.first;
		if (hashSod.find(num) == hashSod.end() /*|| hashSet[num].size() == 0*/)
			throw logged_error(stdPrintf("Digest non trovato per il DG %02X\n", num));
		
		if (hashSod[num] != a.second)
			throw logged_error(stdPrintf("Digest non corrispondente per il DG %02X\n", num));
	}

	/*if (CSCA != null && CSCA.Count > 0)
	{
		log.Info("Verifica catena CSCA");
		X509CertChain chain = new X509CertChain(CSCA);
		var certChain = chain.getPath(certDS);
		if (certChain == null)
			throw Exception("Il certificato di Document Signer non č valido");

		var rootCert = certChain[0];
		if (!new ByteArray(rootCert.SubjectName.RawData).IsEqual(rootCert.IssuerName.RawData))
			throw Exception("Impossibile validare il certificato di Document Signer");
	} */

	return true;
}

/** 
 * Read the NIS contained in this token.
 * @param[out] nisData array in which to store the NIS read back from the token. Should be long enough to contains the entire nis as a C-style sring, i.e. 12 (NIS) + 1 (null terminator) = 13 chars
 * @param[in] callback if @e NULL the call is blocking and the NIS is copied inside @e nisData upon return, otherwise the function spawns a background thread and returns immediately. The thread will invoke the callback funcion passing to it the read NIS
 * @param[in] interval time in ms between polls
 * @param[out] the UID associated to the newly created context of execution
 * @param[in] auth Request this transaction to be verified though one of teh auteyntication method supported by ::AuthType
 * @return 0 on success, negative on error
 * @see stopPoll()
 */
int cie::nis::Token::readNis(char *const nisData, nis_callback_t callback, uint32_t interval, uint32_t *uid, AuthType auth)
{
	int ret = 0;
	const size_t lenData = NIS_LENGTH;
	if(callback && uid) {
		*uid = Utils::getUid();
		shared_ptr<PollExecutor> pollCntr{new PollExecutor{}};

		pollCntr->uid = *uid;
		pollCntr->pollHandle = this;
		pollCntr->callback = callback;
		pollCntr->exitPoll = false;
		pollCntr->pollData = nisData;
		pollCntr->pollSize = lenData;
		pollCntr->interval_ms = interval;
		pollCntr->auth = auth;

		NISManager::getInstance().lockExecutors();
#ifdef USE_EXT_THREAD
		ret = NIS_CreateThread(pollCntr.th, pollNis, weak_ptr<PollExecutor>{pollCntr});
#else
		try {
			pollCntr->th = thread(pollNis, weak_ptr<PollExecutor>{pollCntr});
		} catch(...) {
			cerr << "Cannot create polling thread." << endl;
		}
#endif
		NISManager::getInstance().addExecutor(*uid, pollCntr);

		NISManager::getInstance().unlockExecutors();

		return ret;
	} else {
		string nis = readNis(auth);
		strcpy(nisData, nis.c_str());

		return 0;
	}

	return -1;
}

/** 
 * Read the NIS contained in this token. This method is blocking.
 * @param[in] auth Request this transaction to be verified though one of teh auteyntication method supported by ::AuthType
 * @return a string representing the NIS. It can be empty if the NIS can't be read or if an authentication method has been requested but the auth phase does not succeed
 */
string cie::nis::Token::readNis(AuthType auth)
{
	const size_t lenData = NIS_LENGTH;

	std::vector<BYTE> response(lenData);

	if(Requests::select_df_ias(*this, response)) {
		if(Requests::select_df_cie(*this, response)) {
			if(Requests::read_nis(*this, response)) {
				//check the validity with the SOD
				bool authPassed = true;
				switch(auth)
				{
					case AUTH_PASSIVE:
					case AUTH_INTERNAL:
						ByteDynArray md(CryptoPP::SHA256::DIGESTSIZE);
						std::map<BYTE, ByteDynArray> hashSet;
						if(!cie::nis::utils::digest_SHA256(response.data(), lenData, md.data()))
						{
							authPassed = false;
							cerr << "Error calculating the digest of the file content" << endl;
						}
						else hashSet[0xA1] = md;

						#if true
						ByteDynArray SOD;
						vector<BYTE> sodTmp(1972);	//TODO: hardcoded length is not good
						if(Requests::read_sod(*this, sodTmp)) {
							SOD = ByteArray(sodTmp.data(), sodTmp.size());
							try {
								verifySod(SOD, hashSet);
								//SOD has been verified succeissfully
							} catch (...) {
								authPassed = false;
								cerr << "Error parsing EF.SOD" << endl;
							}
						}
						else {
							authPassed = false;
							cerr << "Error reading EF.SOD" << endl;

						}
						#else
						Sod sod;
						if(parseSod(cie::nis::sodParser, &sod))
						{
							authPassed = false;
							cerr << "Error parsing EF.SOD" << endl;

						}
						if(memcmp(md.data(), sod.idServiceHash, CryptoPP::SHA256::DIGESTSIZE))
							authPassed = false;
						#endif

						break;
				}	
				//----end auth phase-------------
				
				if(authPassed)
					return string{response.begin(), response.end()};
			}
		}
	} 

	return string("");
}

word clamp(const word value, const byte maxValue) {
	if (value > maxValue) {
		return maxValue;
	} else {
		return value;
	}
}

bool cie::nis::Token::readBinaryContent(const cie_EFPath filePath, byte *contentBuffer, word startingOffset, const word contentLength)
{
  #define STATUS_WORD_LENGTH                    (0x02)
  #define PAGE_LENGTH                           (0xE4)
  byte fileId;
  fileId = (byte) (filePath & 0b11111);
  /*switch (filePath.selectionMode) {
    case SELECT_BY_EFID:
      if (!ensureElementaryFileIsSelected(filePath)) {
        return false;
      }
      fileId = 0x00; //Current selected file
    break;

    case SELECT_BY_SFI:
      if (!ensureDedicatedFileIsSelected(filePath.df)) {
        return false;
      }
      fileId = (byte) (filePath.id & 0b11111);
    break;

    default:
      PN532DEBUGPRINT.println(F("The selection mode must be either SELECT_BY_EFID or SELECT_BY_SFI"));
      return false;
  }*/
  bool success = false;
  word offset = startingOffset;
  do {
    word contentPageLength = ::clamp(contentLength+startingOffset-offset, PAGE_LENGTH);
    byte preambleOctets = contentPageLength > 0x80 ? 3 : 2; //Discretionary data: three bytes for responses of length > 0x80
    std::vector<BYTE> readCommand = {
      0x00, //CLA
      0xB1, //INS: READ BINARY (ODD INS)
      0x00, //P1: zeroes
      fileId, //P2: sfi to select or zeroes (i.e. keep current selected file)
      0x04, //Lc: data field is made of 4 bytes
      0x54, 0x02, //Data field: here comes an offset of 2 bytes
      (byte) (offset >> 8), (byte) (offset & 0b11111111), //the offset
      (byte) (contentPageLength + preambleOctets) //Le: bytes to be returned in the response
    };
    word responseLength = ((byte) contentPageLength) + preambleOctets + STATUS_WORD_LENGTH;
    std::vector<BYTE> responseBuffer(responseLength);
    success = transmit(readCommand, responseBuffer) == TOK_RESULT_OK;
    //Copy data over to the buffer
    if (success) {
      //The read binary command with ODD INS incapsulated the response with two or three preamble octets. Don't include them, they're not part of the content.
      //See page 147 in the Gixel manual http://www.unsads.com/specs/IASECC/IAS_ECC_v1.0.1_UK.pdf
      for (word i = preambleOctets; i < contentPageLength + preambleOctets; i++) {
        contentBuffer[offset - startingOffset + i - preambleOctets] = responseBuffer[i];
      }
    }
    offset += contentPageLength;
    
  } while(success && (offset < contentLength));
  if (!success) {
	  std::cout << "Couldn't fetch the elementary file content" << endl;
  }  
  return success;
}

