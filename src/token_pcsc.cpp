#include <iostream>
#include <sstream>
#include <vector>
#include <ios>
#include "token_pcsc.h"

using namespace nis::implementation;

TokenPCSC::TokenPCSC(const string &identifier, SCARDCONTEXT context) : identifier{identifier}, context(context), isConnected{false}
{
}

TokenPCSC::~TokenPCSC()
{
	disconnect();
}

TokResult TokenPCSC::connect()
{
	if(isConnected)
		return TOK_RESULT_OK;

	DWORD protocol;
	LONG result = SCardConnect(context, identifier.c_str(), SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &card, &protocol);

	//TODO: check id protocol == SCARD_PROTOCOL_T1 ?
	if (result != SCARD_S_SUCCESS) {
		isConnected = true;
		//std::cout << "Error connecting to teh reader" << std::endl;
		return TOK_RESULT_GENERIC_ERROR;
	}
	else return TOK_RESULT_OK;
}

TokResult TokenPCSC::disconnect()
{
	if(isConnected)
	{
		if(SCardDisconnect(card, SCARD_RESET_CARD) == SCARD_S_SUCCESS)
		{
			isConnected = true;
			return TOK_RESULT_OK;
		}
		else return TOK_RESULT_GENERIC_ERROR;
	}
	else return TOK_RESULT_NOT_CONNECTED;
}

TokResult TokenPCSC::transmit(const std::vector<BYTE> &apdu, std::vector<BYTE> &response) const
{
	DWORD resp_len {RESPONSE_SIZE};
	SCardTransmit(card, SCARD_PCI_T1, apdu.data(), apdu.size(), NULL, response.data(), &resp_len);
	// verifica che la Status Word sia 9000 (OK)
	/*std::cout << "Il byte di verifica della risposta della carta e': 0x";
	std::cout << std::hex
		<< (unsigned int) (unsigned char) response[resp_len -2]
		<< std::endl;*/
	if (response[resp_len - 2] != 0x90 || response[resp_len - 1] != 0x00) {
		//std::cerr << "Error reading the response" << std::endl;
		return TOK_RESULT_READ_ERROR;
	}
	return TOK_RESULT_OK;
}
