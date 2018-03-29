#ifndef SOD_H_GUARD
#define SOD_H_GUARD

#include <crypto++/sha.h>
#include <stdint.h>
#include "ber/cie_BerTriple.h"
#include "nis_types.h"

namespace cie {
	        namespace nis {

			enum class SodFsm {BEGIN, END, SIGNED_DATA, HASH_TYPES, IDSERVICE_HASH};
			enum class HashType {SHA256};

			struct Sod
			{
				HashType signedDataHash;
				uint8_t idServiceHash[CryptoPP::SHA256::DIGESTSIZE];
			};

			bool sodParser(cie_BerTriple *triple, void *data);
		}
}

#endif
