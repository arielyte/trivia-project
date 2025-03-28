#pragma once
#include <Windows.h>
#include <mutex>
#include <iostream>
#include <map>
#include "LoginRequestHandler.h"
#include "GameRequestHandler.h"
#include "JsonResponsePacketSerializer.h"

#define PORT 8200
#define MESSAGE_CODE_LENGTH_IN_BYTES 1
#define CODE_INDEX 0
#define CHAR_BASE 256
#define NUM_OF_REQUEST_NEED_ONLY_CODE 11
#define NO_MESSAGE 0
#define INDEX_ROOM_ID 0
#define A_CHAR_THAT_MAKE_THE_BUFFER_EXIST 'n' // random char


#define STRING_OF_QUESTION 0
#define STRING_OF_ANSWERS 1
#define STRING_OF_FIRST_ANSWER 0
#define STRING_OF_SECOND_ANSWER 1
#define STRING_OF_THIRD_ANSWER 2
#define STRING_OF_FOURTH_ANSWER 3
#define DOT '.'
#define COMMA ','

#define INDEX_OF_FAILES_OR_SUCCED 0
#define INDEX_CORRECT_ANSWER_ID 0


class Communicator {
public:
	// methods
	void startHandleRequests();

	//c'tor
	Communicator();

	//d'tor
	~Communicator();

	// getters
	RequestHandlerFactory& getHandlerFactory();


private:
	// fields
	SOCKET m_serverSocket;
	std::map<SOCKET, IRequestHandler*> m_clients;
	std::map<std::string, SOCKET> m_players;
	RequestHandlerFactory m_handlerFactory;

	// helper methods
	void handleClientMessage(const SOCKET& sc);
	void sendError(const SOCKET& sc, const std::string exception);
	int charToInt(const char* bits, const int& size);
	void bindAndListen();
	void acceptNewClient();
	void handleNewClient(SOCKET clientSocket);
	template<class Response>
	static void sendResponse(const SOCKET& sc, Response& response);
};