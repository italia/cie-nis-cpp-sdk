#include <iostream>
#include <sstream>
#include <vector>
//extern "C"{
#include <winscard.h>
//}
#include <ios>
#include "requests.h"

bool Requests::send_apdu(const Token &card,
		const std::vector<BYTE> &apdu, std::vector<BYTE> &response)
{
	if(card.transmit(apdu, response) == TOK_RESULT_OK)
		return true;

	return false;
}

bool Requests::select_df_ias(const Token &card, std::vector<BYTE> &response)
{
	std::vector<BYTE> selectIAS {0x00, // CLA
		0xa4, // INS = SELECT FILE
		0x04, // P1 = Select By AID
		0x0c, // P2 = Return No Data
		0x0d, // LC = length of AID
		0xA0, 0x00, 0x00, 0x00, 0x30, 0x80, 0x00, 0x00,
		0x00, 0x09, 0x81, 0x60, 0x01 // AID
	};
	// invia l'APDU
	if (!Requests::send_apdu(card, selectIAS, response)) {
		std::cerr << "Errore nella selezione del DF_IAS\n";
		return false;
	} 
	return true;
}

bool Requests::select_df_cie(const Token &card, std::vector<BYTE> &response)
{
	std::vector<BYTE> selectCIE {0x00, // CLA
		0xa4, // INS = SELECT FILE
		0x04, // P1 = Select By AID
		0x0c, // P2 = Return No Data
		0x06, // LC = length of AID
		0xA0, 0x00, 0x00, 0x00, 0x00, 0x39 // AID
	};
	// invia la seconda APDU
	if (!Requests::send_apdu(card, selectCIE, response)) {
		std::cerr << "Errore nella selezione del DF_CIE\n";
		return false;
	} 
	return true;
}

bool Requests::mse_set(const Token &card, std::vector<BYTE> &response)
{
	std::vector<BYTE> setMSE {0x00, // CLA
		0x22, // INS = MSE SET 
		0x41, // P1
		0xA4, // P2 
		0x06, // LC = length of payload
		0x80, 0x01, 0x02, 0x84, 0x01, 0x83 // select RSA algorithm and key no. 3
	};
	// invia la seconda APDU
	if (!Requests::send_apdu(card, setMSE, response)) {
		std::cerr << "Errore nell'impostazione di algoritmo e chiave per il challenge/response\n";
		return false;
	} 
	return true;
}

bool Requests::internal_authenticate(const Token &card, const std::vector<BYTE> &challenge, std::vector<BYTE> &response)
{
	std::vector<BYTE> setMSE {0x00, // CLA
		0x88, // INS = INTERNAL AUTHENTICATE
		0x00, // P1 = no info given for the algorithm, 
		0x00, // P2 = no info given for the secret
		(BYTE)challenge.size() // LC = length of payload
	};
	setMSE.insert(setMSE.end(), challenge.begin(), challenge.end());
	setMSE.push_back(0);	//Le = read all you can
	// invia la seconda APDU
	if (!Requests::send_apdu(card, setMSE, response)) {
		std::cerr << "Errore nell'impostazione di algoritmo e chiave per il challenge/response\n";
		return false;
	} 
	return true;
}

/*bool Requests::read_nis(const Token &card, std::vector<BYTE> &response)
{
	std::vector<BYTE> readNIS = {0x00, // CLA
		0xb0, // INS = READ BINARY
		0x81, // P1 = Read by SFI & SFI = 1 //to read public key
		0x00, // P2 = Offset = 0
		0x0c  // LE = length of NIS
	};
	// invia l'APDU
	if (!Requests::send_apdu(card, readNIS, response)) {
		std::cerr << "Errore nella lettura dell'Id_Servizi\n";
		return false;
	} 
	return true;
}

bool Requests::read_sod(const Token &card, std::vector<BYTE> &ret)
{
	std::vector<BYTE> response(2);
 	if(Requests::select_df_ias(card, response))
	{
 		if(Requests::select_df_cie(card, response))
		{
			const uint16_t fileId = 0x1006;
			std::vector<BYTE> selectSOD = {0x00, // CLA
				0xa4, // INS = SELECT FILE
				0x02, // P1 = select bu EFID under current DF
				0x0c, // P2 = return no data
				0x02, // LE = length of following data
				fileId >> 8,	//high byte of EFID
				fileId & 0xFF	//low byte of EFID
			};
			// invia l'APDU
			if (!Requests::send_apdu(card, selectSOD, response)) {
				std::cerr << "Errore nella lettura del SOD\n";
				return false;
			}

			const_cast<Token&>(card).readBinaryContent(0x00, ret, 0, -1);	//TODO: this cast is ugly
		}
	}

	return true;
}*/

bool Requests::read_EF_file(const Token &card, const Efid efid, std::vector<BYTE> &ret)
{
	std::vector<BYTE> response(2);
 	if(Requests::select_df_ias(card, response))
	{
 		if(Requests::select_df_cie(card, response))
		{
			const uint16_t fileId = efid;
			//////////////////////////////////////	
			//TODO: this should be the correct way to obtain the file size, in order not to read out of the bound of teh file
			/*std::vector<BYTE> s = {0x00, // CLA
				0xa4, // INS = SELECT FILE
				0x02, // P1 = select bu EFID under current DF
				0x00, // P2 = returnFCI
				0x02, // LE = length of following data
				fileId >> 8,	//high byte of EFID
				fileId & 0xFF	//low byte of EFID
			};
			// invia l'APDU
			if (!Requests::send_apdu(card, s, response)) {
				std::cerr << "Errore nella lettura del SOD\n";
			}

			std::vector<BYTE> sS = {0x00, // CLA
				0xC0, // INS = GET RESPONSE
				0x00, // P1 = select bu EFID under current DF
				0x00, // P2 = return no data
				0x19, // LE = length of following data
			};
			// invia l'APDU
			if (!Requests::send_apdu(card, sS, response)) {
				std::cerr << "Errore nella lettura del SOD\n";
			}
			*/
			//////////////////////////////////////	
			std::vector<BYTE> selectIntKpub = {0x00, // CLA
				0xa4, // INS = SELECT FILE
				0x02, // P1 = select bu EFID under current DF
				0x0c, // P2 = return no data
				0x02, // LE = length of following data
				(BYTE)(fileId >> 8),	//high byte of EFID
				(BYTE)(fileId & 0xFF)	//low byte of EFID
			};
			// invia l'APDU
			if (!Requests::send_apdu(card, selectIntKpub, response)) {
				std::cerr << "Errore nella lettura di Service_Int.Kpub\n";
				return false;
			}

			const_cast<Token&>(card).readBinaryContent(0x00, ret, 0, -1);	//TODO: this cast is ugly
		}
	}

	return true;
}

bool Requests::create_apdu(std::vector<BYTE> &apdu)
{
	std::string apdu_string {};
	std::cout << "Inserisci i valori dell'APDU: ";
	std::cout.flush();
	std::cin >> apdu_string;
	//std::getline(std::cin, apdu_string);
	std::istringstream sstream {apdu_string};

	for (std::string tmp {}; std::getline(sstream, tmp, '-' );)
		apdu.push_back((BYTE) std::stoi(tmp));
	std::cout << apdu_string << '\n';
	std::cout << "Invio dell'APDU personalizzata..." << '\n';
	return true;
}

bool Requests::start_interactive_session(const Token &card)
{
	std::vector<BYTE> apdu {};
	std::vector<BYTE> response {};
	bool is_good_response {true};
	while (is_good_response) {
		Requests::create_apdu(apdu);
		is_good_response = Requests::send_apdu(card, apdu, response);
		std::cout << std::endl;
		if (!is_good_response) {
			std::cerr << "Errore nella lettura dell'APDU personalizzata\n";
			return false;
		} 
		std::cout << "output message:" << std::string {(char *)response.data()} << std::endl;
	}
	return true;
}
