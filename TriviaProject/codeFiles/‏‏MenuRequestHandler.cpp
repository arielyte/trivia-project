#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "RoomHelper.h"



// c'tor
MenuRequestHandler::MenuRequestHandler(const LoggedUser& user, RequestHandlerFactory& handlerFactoryOfAllPlayers) :m_handlerFactory(handlerFactoryOfAllPlayers)
{
	// declare what the client in this state of menu can do
	this->relevantRequestsIds.push_back(REQUEST_GET_PLAYERS_IN_ROOM_CODE);
	this->relevantRequestsIds.push_back(REQUEST_JOIN_ROOM_CODE);
	this->relevantRequestsIds.push_back(REQUEST_CREATE_ROOM_CODE);
	this->relevantRequestsIds.push_back(REQUEST_GET_ROOMS_CODE);
	this->relevantRequestsIds.push_back(REQUEST_LOGOUT_CODE);
	this->relevantRequestsIds.push_back(REQUEST_GET_STATISTICS_CODE);
	this->relevantRequestsIds.push_back(REQUEST_GET_HIGH_SCORE_CODE);
	

	// set the user
	this->m_user = user;
}



// handle a request by menu handeling format
RequestResult MenuRequestHandler::handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{

	// set
	RequestResult result;

	// check if the request is relevant
	if (!isRequestRelevant(requestInfo))
	{
		throw std::exception("Your action is not relevant to your current state");
	}

	try
	{
		// Check what is the request. And active the right method to treat the request
		switch (requestInfo.RequestId)
		{
		case REQUEST_GET_PLAYERS_IN_ROOM_CODE:
		{
			result = this->getPlayersInRoom(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_JOIN_ROOM_CODE:
		{
			result = this->joinRoom(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_CREATE_ROOM_CODE:
		{
			result = this->createRoom(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_GET_ROOMS_CODE:
		{
			result = this->getRooms(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_LOGOUT_CODE:
		{
			result = this->logout(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_GET_STATISTICS_CODE:
		{
			result = this->getPersonalStats(requestInfo);
			break;
		}

		case REQUEST_GET_HIGH_SCORE_CODE:
		{
			result = this->getHighScore(requestInfo);
			break;
		}
		}
	}
	catch (std::exception& e)
	{
		throw e;
	}

	return result;
}



// logout the user
RequestResult MenuRequestHandler::logout(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory) const
{
	// logout
	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	m_handlerFactory.getLoginManager().logout(m_user.getUsername());
	// set
	RequestResult result;
	// set the newHndler to be login
	result.newHandler = this->m_handlerFactory.createLoginRequestHandler();
	handlerFactoryLock.unlock();

	for (int i = FIRST; i < m_user.getUsername().size(); i++)
	{
		result.response.push_back(m_user.getUsername()[i]);
	}

	return result;
}

// add the user to a room
RequestResult MenuRequestHandler::joinRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set
	Room* roomToErnter;
	// desrialize the message
	JoinRoomRequest joinRoomRequest = JsonResponsePacketDeserializer::deserializeJoinRoomRequest(requestInfo.buffer);

	try
	{
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		// get the room that the user want to get in
		roomToErnter = &m_handlerFactory.getRoomManager().getRoom(joinRoomRequest.roomID);
	}
	catch (std::exception& e)
	{
		throw e;
	}

	try
	{
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		// add the user to the room
		roomToErnter->addUser(m_user);
	}
	catch (std::exception& e)
	{
		throw e;
	}


	// set
	RequestResult result;
	// set the newHndler to be room member
	result.newHandler = this->m_handlerFactory.createRoomMemberRequestHandler(m_user, *roomToErnter);

	return result;
}

// create new room to the user
RequestResult MenuRequestHandler::createRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// desrialize the message
	CreateRoomRequest createRoomRequest = JsonResponsePacketDeserializer::deserializeCreateRoomRequest(requestInfo.buffer);

	// set the roomData by the request
	RoomData roomData;
	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	roomData.id = m_handlerFactory.getRoomManager().getIdForRoom();
	handlerFactoryLock.unlock();
	roomData.isActive = ACTIVE; // there is player in the room that wait/in a game
	roomData.maxPlayers = createRoomRequest.maxUsers;
	roomData.name = createRoomRequest.roomName;
	roomData.numOfQuestionsInGame = createRoomRequest.questionCount;
	roomData.timePerQuestion = createRoomRequest.answerTimeout;


	try
	{
		// create the room
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		m_handlerFactory.getRoomManager().createRoom(this->m_user, roomData);
	}
	catch (std::exception& e)
	{
		throw e;
	}

	// set
	RequestResult result;
	// set the newHndler to be menu
	result.newHandler = this->m_handlerFactory.createRoomAdminRequestHandler(m_user, m_handlerFactory.getRoomManager().getRoom(roomData.id));

	return result;
}


// get all the rooms that exist
RequestResult MenuRequestHandler::getRooms(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// get the rooms as vector of bytes
	std::vector<BYTE> roomsInBytes = RoomHelper::getRoomsInBytes(m_handlerFactory, lockHandlerFactory);

	// set
	RequestResult result;
	// set the newHndler to be menu
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(m_user);

	//set the response to be personal stats
	result.response = roomsInBytes;

	return result;
}

// get all the players in the room
RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set
	std::vector<BYTE> playersInBytes;
	std::vector<std::string> players;

	// desrialize the message
	GetPlayersInRoomRequest getPlayersInRoomRequest = JsonResponsePacketDeserializer::deserializeGetPlayersInRoomRequest(requestInfo.buffer);

	try
	{
		// get the players in room
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		players = m_handlerFactory.getRoomManager().getRoom(getPlayersInRoomRequest.roomID).getAllUsersAsStrings();
	}
	catch (std::exception& e)
	{
		throw e;
	}

	// convert the players vector to vector of bytes
	for (std::string str : players)
	{
		for (char c : str)
		{
			playersInBytes.push_back(c);
		}
		playersInBytes.push_back(',');
		//playersInBytes.push_back(' ');
	}

	// remove the last comma and space that not nedded
	if (playersInBytes.size() > ONE_COMMA) // if there is no players - there is no vector and you can't pop back
	{
		playersInBytes.pop_back();
		//playersInBytes.pop_back();
	}

	// set
	RequestResult result;
	// set the newHndler to be menu
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(m_user);

	//set the response to be players
	result.response = playersInBytes;

	return result;
}

// get the user stats
RequestResult MenuRequestHandler::getPersonalStats(const RequestInfo& requestInfo)
{
	// set
	std::vector<std::string> stats;
	std::vector<BYTE> statsInBytes;

	// get the personal stats
	try
	{
		stats = m_handlerFactory.getStatisticsManager().getUserStatistics(this->m_user.getUsername());
	}
	catch (std::exception& e)
	{
		throw e;
	}

	// convert the personal stats vector to vector of bytes
	for (std::string str : stats)
	{
		for (char c : str)
		{
			statsInBytes.push_back(c);
		}
		statsInBytes.push_back(',');
	}

	// remove the last comma and space that not nedded
	if (statsInBytes.size() > ONE_COMMA) // if there is no stats - there is no vector and you can't pop back
	{
		statsInBytes.pop_back();
	}

	// set
	RequestResult result;
	// set the newHndler to be menu
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(m_user);

	//set the response to be personal stats
	result.response = statsInBytes;

	return result;
}

// get the hightest scores in the game
RequestResult MenuRequestHandler::getHighScore(const RequestInfo& requestInfo)
{
	// set
	std::vector<int> highScores;
	std::vector<BYTE> highScoreInBytes;

	// get the personal stats
	try
	{
		// get the high score
		highScores = m_handlerFactory.getStatisticsManager().getHighScore();
	}
	catch (std::exception& e)
	{
		throw e;
	}

	// convert the high score vector to vector of bytes
	for (int score : highScores)
	{
		std::string stringScore = std::to_string(score);

		for (char c : stringScore)
		{
			highScoreInBytes.push_back(c);
		}
		highScoreInBytes.push_back(',');
	}

	// remove the last comma and space that not nedded
	if (highScoreInBytes.size() > ONE_COMMA) // if there is no hih score - there is no vector and you can't pop back
	{
		highScoreInBytes.pop_back();
	}

	// set
	RequestResult result;
	// set the newHndler to be menu
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(m_user);

	//set the response to be high score
	result.response = highScoreInBytes;

	return result;
}