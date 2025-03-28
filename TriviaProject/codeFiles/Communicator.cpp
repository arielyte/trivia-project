#include "Communicator.h"


// global mutexes
std::mutex lockCerr;
std::mutex lockClients;
std::mutex lockPlayers;
std::mutex lockHandlerFactory;



//c'tor
Communicator::Communicator()
{
	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// if creating the socket wasn't succeed
	if (m_serverSocket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__ " - socket");
	}
}

// getter for the handler factory
RequestHandlerFactory& Communicator::getHandlerFactory()
{
	return this->m_handlerFactory;
}


//d'tor
Communicator::~Communicator()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(m_serverSocket);
	}
	catch (...) {}

	// free the handlers
	for (std::pair<SOCKET, IRequestHandler*> pair : m_clients)
	{
		delete pair.second;
	}
}


// start the communicator working
void Communicator::startHandleRequests()
{
	try
	{
		// bind the server and listen for new clients
		bindAndListen();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}


// bind the server and listen for new clinets
void Communicator::bindAndListen()
{
	// set
	struct sockaddr_in sa = { 0 };

	// bind
	sa.sin_port = htons(PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - bind");
	}

	// Start listening for incoming requests of clients
	if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - listen");
	}

	std::cout << "Listening on port " << PORT << std::endl;

	// endless loop to accept new clients
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "Waiting for client connection request" << std::endl;
		acceptNewClient();
	}
}

// accep new client
void Communicator::acceptNewClient()
{
	// this accepts the client and create a specific socket from server to this client
	// the process will not continue until a client connects to the server
	SOCKET client_socket = accept(m_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__);
	}

	std::cout << "Client accepted. Server and client can speak" << std::endl;

	// insert the client into the map of clients with Handle of login
	std::unique_lock<std::mutex> clientsLock(lockClients);
	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	m_clients.insert({ client_socket, m_handlerFactory.createLoginRequestHandler() });
    clientsLock.unlock();
    handlerFactoryLock.unlock();

	// create thread for the client
	std::thread handle(&Communicator::handleNewClient, this, client_socket);
	handle.detach();
}

// hansle new client
void Communicator::handleNewClient(SOCKET clientSocket)
{
	while (true)
	{
        try
        {
            // handle a message
            handleClientMessage(clientSocket);
        }
		catch (std::exception& e)
		{
			std::lock_guard<std::mutex> sendError(lockCerr);
			std::cerr << e.what() << std::endl;
		}
	}
}





void Communicator::handleClientMessage(const SOCKET& sc)
{
    // Set
    const int ONLY_CODE_REQUESTS[NUM_OF_REQUEST_NEED_ONLY_CODE] = { 106, 107, 108, 109, 110, 111, 112, 113, 115, 116, 117 };
    char messageCodeArray[MESSAGE_CODE_LENGTH_IN_BYTES + 1] = { 0 }; // +1 for an '/0'
    char dataLengthArray[DATA_LENGTH_IN_BYTES + 1] = { 0 }; // +1 for an '/0'
    char* dataArray;
    int messageCode = 0;
    int dataLength = 0;
    int resultOfRecv = 0;
    RequestInfo requestInfo;
    RequestResult result;
    // lock the entering/exiting the clients map
    std::unique_lock<std::mutex> clientsLock(lockClients);
    auto client = m_clients.find(sc);
    clientsLock.unlock();

    // Get into messageCode, the code of the message
    resultOfRecv = recv(sc, messageCodeArray, MESSAGE_CODE_LENGTH_IN_BYTES, 0);
    if (resultOfRecv == INVALID_SOCKET || resultOfRecv == NO_MESSAGE)
    {
        return;
    }

    // check if the request has a data or not, and act as need
    messageCode = (int)(messageCodeArray[CODE_INDEX]); // convera the message code to int
    // set the code to the requestInfo
    requestInfo.RequestId = messageCode;
    // check if the code in the array of the only codes nedded (not data)
    if (std::find(std::begin(ONLY_CODE_REQUESTS), std::end(ONLY_CODE_REQUESTS), messageCode) == std::end(ONLY_CODE_REQUESTS))
    {
        // check if the massege code is 0, because there is last of message, that make the server recv again a message, that was't sent ever. the message code of it is 0
        if (messageCode == NO_MESSAGE)
        {
            return;
        }
        // Get into messageLength, the length of the message
        resultOfRecv = recv(sc, dataLengthArray, DATA_LENGTH_IN_BYTES, 0);
        if (resultOfRecv == INVALID_SOCKET)
        {
            std::string error = "Error while receiving from socket: ";
            error += std::to_string(sc);
            throw std::exception(error.c_str());
        }

        // Get the data length (convert from string of bits to an int)
        dataLength = charToInt(dataLengthArray, DATA_LENGTH_IN_BYTES);

        // Get the data
        // Allocate memory to array of chars for getting the data.
        dataArray = new char[dataLength];
        resultOfRecv = recv(sc, dataArray, dataLength, 0);
        if (resultOfRecv == INVALID_SOCKET)
        {
            std::string s = "Error while receiving from socket: ";
            s += std::to_string(sc);
            delete[] dataArray; // Free memory
            throw std::exception(s.c_str());
        }


        // Pushback the array of chars of the data, to the requestInfo vector
        // and convert dataArray to a string of the message
        for (int i = 0; i < dataLength; i++)
        {
            requestInfo.buffer.push_back(dataArray[i]);
        }

        // Free memory
        delete[] dataArray;
    }
    else
    {
        // make the buffer to exsist
        requestInfo.buffer.push_back(A_CHAR_THAT_MAKE_THE_BUFFER_EXIST);
    }


    // set the creation time to the requestInfo
    auto now = std::chrono::system_clock::now();
    requestInfo.receivalTime = std::chrono::system_clock::to_time_t(now);


    // find the handler of the client and handle the request with the client handler
    try
    {
        result = client->second->handleRequest(requestInfo, lockHandlerFactory);
    }
    catch (std::exception& e)
    {
        // send ErrorRespone to the client
        sendError(sc, e.what());
        // throw the exception up in the stack
        throw e;
    }
    // lock the entering/exiting the clients map
    clientsLock.lock();
    // insert into the map of clients the new handler, after deleting the previos
    delete m_clients.find(sc)->second;
    m_clients.find(sc)->second = result.newHandler;
    clientsLock.unlock();


    // set the response to send to the client
    switch (messageCode)
    {
    case REQUEST_LOGIN_CODE:
    {
        std::string username(result.response.begin(), result.response.end());

        // lock the entering/exiting the players map
        std::unique_lock<std::mutex> playersLock(lockPlayers);
        m_players.insert({ username, sc });
        playersLock.unlock();


        // set
        LoginResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_SIGNUP_CODE:
    {
        // set
        SignupResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_GET_PLAYERS_IN_ROOM_CODE:
    {
        // set
        std::string players(result.response.begin(), result.response.end());
        GetPlayersInRoomResponse response;
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        response.players.push_back(players);

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_JOIN_ROOM_CODE:
    {
        // set
        JoinRoomResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_CREATE_ROOM_CODE:
    {
        // set
        CreateRoomResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_GET_ROOMS_CODE:
    {
        // set
        std::vector <RoomData> rooms;
        RoomData roomData;
        int numOfCommasPassed = 0;
        // intitialize
        roomData.name = "";

        // get from the result response the rooms data
        for (BYTE byte : result.response)
        {
            if (byte == '.')
            {
                rooms.push_back(roomData);
                numOfCommasPassed = 0;
                roomData.name = "";
                continue;
            }
            if (byte == ',')
            {
                ++numOfCommasPassed;
                continue;
            }
            if (numOfCommasPassed == 0)
            {
                roomData.id = byte;
                continue;
            }
            if (numOfCommasPassed == 1)
            {
                roomData.isActive = byte;
                continue;
            }
            if (numOfCommasPassed == 2)
            {
                roomData.maxPlayers = byte;
                continue;
            }
            if (numOfCommasPassed == 3)
            {
                roomData.name += byte;
                continue;
            }
            if (numOfCommasPassed == 4)
            {
                roomData.numOfQuestionsInGame = byte;
                continue;
            }
            if (numOfCommasPassed == 5)
            {
                roomData.timePerQuestion = byte;
                continue;
            }
        }

        GetRoomsResponse response;
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        response.rooms = rooms;
        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_LOGOUT_CODE:
    {
        std::string username(result.response.begin(), result.response.end());
        // lock the entering/exiting the players map
        std::unique_lock<std::mutex> playersLock(lockPlayers);
        m_players.erase(username);
        playersLock.unlock();

        // set
        LogoutResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_GET_STATISTICS_CODE:
    {
        // set
        std::vector <std::string> statistics;
        std::string allStats(result.response.begin(), result.response.end());
        statistics.push_back(allStats);

        GetPersonalStatsResponse response;
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        response.statistics = statistics;
        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }

    case REQUEST_GET_HIGH_SCORE_CODE:
    {
        // set
        std::vector <std::string> statistics;
        std::string allScores(result.response.begin(), result.response.end());
        statistics.push_back(allScores);

        GetHighScoreResponse response;
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        response.statistics = statistics;

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    case REQUEST_CLOSE_ROOM_CODE:
    {
        // set
        CloseRoomResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        std::vector<std::string> idAndPlayersInRoom;
        std::string IdOrPlayer = "";

        // get all the players that in the room and the room id
        LeaveRoomResponse leaveRoomResponse{ RESPONSE_OPERATION_WORKED_CODE };
        for (char letter : result.response)
        {
            if (letter == ',')
            {
                idAndPlayersInRoom.push_back(IdOrPlayer);
                IdOrPlayer = "";
            }
            else
            {
                IdOrPlayer += letter;
            }
        }
        for (std::string username : idAndPlayersInRoom)
        {
            // move on if the username - is the ID of the room
            if (username == idAndPlayersInRoom[INDEX_ROOM_ID])
            {
                continue;
            }

            // send the user that he need to leave the room that closses
            std::lock_guard<std::mutex> playersLock(lockPlayers);
            sendResponse(m_players.find(username)->second, leaveRoomResponse);

            // delete it's handler
            std::lock_guard<std::mutex> clientsLock(lockClients);
            delete m_clients.find(m_players.find(username)->second)->second;
            std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
            
            // set him a new handler
            m_clients.find(m_players.find(username)->second)->second = this->m_handlerFactory.createMenuRequestHandler(m_handlerFactory.getLoginManager().findUserByName(username));
        }
        // delete the room
        std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
        m_handlerFactory.getRoomManager().deleteRoom(atoi(idAndPlayersInRoom[INDEX_ROOM_ID].c_str()));
        handlerFactoryLock.unlock();

        // send a massege to the client that all went well
        sendResponse(sc, response);

        break;
    }
    case REQUEST_START_GAME_CODE:
    {
        // set
        StartGameResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        std::vector<std::string> idAndPlayersInRoom;
        std::string IdOrPlayer = "";

        // get all the players that in the room and the room id
        StartGameResponse startGameResponse{ RESPONSE_OPERATION_WORKED_CODE };
        for (char letter : result.response)
        {
            if (letter == ',')
            {
                idAndPlayersInRoom.push_back(IdOrPlayer);
                IdOrPlayer = "";
            }
            else
            {
                IdOrPlayer += letter;
            }
        }
        for (std::string username : idAndPlayersInRoom)
        {
            // move on if the username - is the ID of the room
            if (username == idAndPlayersInRoom[INDEX_ROOM_ID])
            {
                continue;
            }

            // send the user that he need to leave the room that closses
            std::lock_guard<std::mutex>playersLock(lockPlayers);
            sendResponse(m_players.find(username)->second, startGameResponse);
            std::lock_guard<std::mutex> clientsLock(lockClients);
            delete m_clients.find(m_players.find(username)->second)->second;
            std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
            m_clients.find(m_players.find(username)->second)->second = this->m_handlerFactory.createGameRequestHandler(m_handlerFactory.getLoginManager().findUserByName(username));
        }

        // send a massege to the client that all went well
        sendResponse(sc, response);

        break;
    }
    case REQUEST_LEAVE_ROOM_CODE:
    {
        // set
        LeaveRoomResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    case REQUEST_GET_ROOM_STATE_CODE:
    {
        // set
        std::string roomIDString(result.response.begin(), result.response.end());
        int roomID = atoi(roomIDString.c_str());
        Room room = m_handlerFactory.getRoomManager().getRoom(roomID);
        bool hasGameBegun = false;
        // check if the game begun
        if (room.getRoomData().isActive == GAME_BUGUN)
        {
            hasGameBegun = true;
        }

        GetRoomStateResponse response;
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        response.answerTimeout = room.getRoomData().timePerQuestion;
        response.hasGameBegun = hasGameBegun;
        response.players = room.getAllUsersAsStrings();
        response.questionCount = room.getRoomData().numOfQuestionsInGame;

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    case REQUEST_GET_QUESTION_REQUEST_CODE:
    {
        // set
        int dotsPassed = 0;
        int commasPassed = 0;
        std::string answer = "";
        GetQuestionResponse response{};
        response.question = "";
        response.status = result.response[INDEX_OF_FAILES_OR_SUCCED]; // set the response to be 'a success response to be what it need to be

        // set a new vector of the response that so not include the status code
        std::vector<char> resultResponse(result.response.begin() + INDEX_OF_FAILES_OR_SUCCED + NEXT, result.response.end());

        // get the result response and convert it into GetQuestionResponse
        for (char letter : resultResponse)
        {
            // if we getting into dot - update the num of dots we had passed
            if (letter == DOT)
            {
                ++dotsPassed;
                continue;
            }

            // if we did'nt pass any dot - all the letters are parts of the question string
            if (dotsPassed == STRING_OF_QUESTION)
            {
                response.question += letter;
                continue;
            }

            // if we passed onr dot - all the letters are parts of the question's answers
            if (dotsPassed == STRING_OF_ANSWERS)
            {
                // if we getting into comma - update the num of commas we had passed
                // and insert the answer into the response
                if (letter == COMMA)
                {
                    ++commasPassed;
                    response.answers.insert({ commasPassed,answer });
                    answer = ""; // resset
                }
                else
                {
                    answer += letter;
                }
            }
        }
        
        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    case REQUEST_SUBMIT_ANSWER_CODE:
    {
        // set
        SubmitAnswerRespone response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response
        response.correctAnswerId = result.response[INDEX_CORRECT_ANSWER_ID];

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    case REQUEST_LEAVE_GAME_REQUEST_CODE:
    {
        // set
        LeaveGameResponse response{};
        response.status = RESPONSE_OPERATION_WORKED_CODE; // set the response to be a success response

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    case REQUEST_GET_GAME_RESULT_REQUEST_CODE:
    {
        // set
        GetGameResultsResponse response{};
        PlayerResults playerResults{ "",0,0,0 };
        int numOfCommasPassed = 0;

        response.status = result.response[INDEX_OF_FAILES_OR_SUCCED]; // set the response to be 'a success response to be what it need to be

        // set a new vector of the response that so not include the status code
        std::vector<char> resultResponse(result.response.begin() + INDEX_OF_FAILES_OR_SUCCED + NEXT, result.response.end());

        // get over all the chars in the response
        for (char letter : resultResponse)
        {
            if (letter == '.')
            {
                response.results.push_back(playerResults);
                numOfCommasPassed = 0;
                playerResults.username = "";
                continue;
            }
            if (letter == ',')
            {
                ++numOfCommasPassed;
                continue;
            }
            if (numOfCommasPassed == 0)
            {
                playerResults.username += letter;
                continue;
            }
            if (numOfCommasPassed == 1)
            {
                playerResults.averageAnswerTime = letter;
                continue;
            }
            if (numOfCommasPassed == 2)
            {
                playerResults.correctAnswerCount = letter;
                continue;
            }
            if (numOfCommasPassed == 3)
            {
                playerResults.worngAnswerCount = letter;
                continue;
            }
        }

        // send a massege to the client that all went well
        sendResponse(sc, response);
        break;
    }
    }
}



template<class Response>
void Communicator::sendResponse(const SOCKET& sc, Response& response)
{
    // set
    std::vector<unsigned char> buffer;

    // serilize the response
    buffer = JsonResponsePacketSerializer::serializeResponse(response);

    // convert buffer to string
    std::string data(buffer.begin(), buffer.end());


    // send respone to the client
    if (send(sc, data.c_str(), data.size(), 0) == INVALID_SOCKET)
    {
        throw std::exception("Error while sending message to client");
    }
}



void Communicator::sendError(const SOCKET& sc, const std::string exception)
{
    // set
    ErrorResponse error;
    error.message = exception;
    std::vector<unsigned char> buffer = JsonResponsePacketSerializer::serializeResponseError(error);

    // convert buffer to string
    std::string data(buffer.begin(), buffer.end());

    // send respone to the client
    if (send(sc, data.c_str(), data.size(), 0) == INVALID_SOCKET)
    {
        throw std::exception("Error while sending message to client");
    }
}




// convert binary string to int
int Communicator::charToInt(const char* bits, const int& size)
{
    // set
    int sumBits = 0;
    int power = 0;

    // get over all the bits and sum them to int. moving from left to right to get the bit value. moving from right to left to get the power of this bit (the index from end)
    for (int bitIndex = size - 1; bitIndex >= FIRST; bitIndex--)
    {
        // sum the bits to an int
        sumBits += (bits[bitIndex]) * pow(CHAR_BASE, power);
        ++power;
    }

    return sumBits;
}