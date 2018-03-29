#include <memory>
#include "token.h"
#include "executor.h"
#include "uid.h"
#include "nis_manager.h"
#include "digest.h"
#include "ber/cie_BerReader.h"

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

int cie::nis::Token::parseSod(cieBerTripleCallbackFunc callback, Sod *sod)
{
	unsigned int lenData = 4096;

	std::vector<BYTE> response(lenData);

	if(Requests::select_df_ias(*this, response)) {
		if(Requests::select_df_cie(*this, response)) {
			cie_BerReader ber{this, sod};
			return ber.readTriples(0x06/*SOD SFI*/, callback, &lenData, 30/*depth*/) ? 0 : -1;
		}
	}

	return -2;	
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
						uint8_t md[CryptoPP::SHA256::DIGESTSIZE];
						if(!cie::nis::utils::digest_SHA256(response.data(), lenData, md))
						{
							authPassed = false;
							cerr << "Error calculating the digest of the file content" << endl;
						}

						Sod sod;
						if(parseSod(cie::nis::sodParser, &sod))
						{
							authPassed = false;
							cerr << "Error parsing EF.SOD" << endl;

						}

						if(memcmp(md, sod.idServiceHash, sizeof(md)))
							authPassed = false;
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

