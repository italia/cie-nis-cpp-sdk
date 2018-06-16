#include <iostream>
#include <sstream>
#include <vector>
#include <ios>
#include "token_pcsc.h"

using namespace cie::nis;

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

TokResult TokenPCSC::transmit(const std::vector<BYTE> &apdu, std::vector<BYTE> &response, size_t *retlen) const
{
	size_t resp_len = response.size();
	SCardTransmit(card, SCARD_PCI_T1, apdu.data(), apdu.size(), nullptr, response.data(), &resp_len);
	if(retlen)
		*retlen = resp_len;
	
	if (response[resp_len - 2] == 0x90 && response[resp_len - 1] == 0x00) 
		return TOK_RESULT_OK;
	else if (response[resp_len - 2] == 0x62 && response[resp_len - 1] == 0x82)
		return TOK_RESULT_EOF;
	else if (response[resp_len - 2] == 0x6C) {
		*retlen = response[resp_len - 1];
		return TOK_RESULT_WRONG_LENGTH;
	}
	else if (response[resp_len - 2] == 0x6B && response[resp_len - 1] == 0x00)
		return TOK_RESULT_OFFSET_OOB;
	else return TOK_RESULT_READ_ERROR;
}
