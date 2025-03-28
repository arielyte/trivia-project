#include "RoomAdminRequestHandler.h"
#include "MenuRequestHandler.h"
#include "GameRequestHandler.h"
#include "RoomHelper.h"


// c'tor
RoomAdminRequestHandler::RoomAdminRequestHandler(const LoggedUser& user, Room& room, RequestHandlerFactory& handlerFactoryOfAllPlayers) : m_room(room), m_handlerFactory(handlerFactoryOfAllPlayers), m_roomManager(handlerFactoryOfAllPlayers.getRoomManager())
{
	// declare what the client in this state of room admin can do
	this->relevantRequestsIds.push_back(REQUEST_GET_ROOM_STATE_CODE);
	this->relevantRequestsIds.push_back(REQUEST_CLOSE_ROOM_CODE);
	this->relevantRequestsIds.push_back(REQUEST_START_GAME_CODE);
	this->relevantRequestsIds.push_back(REQUEST_GET_ROOMS_CODE);

	this->m_user = user;
}

// handle a request by room admin handeling format
RequestResult RoomAdminRequestHandler::handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
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
		case REQUEST_GET_ROOM_STATE_CODE:
		{
			result = this->getRoomState(requestInfo);
			break;
		}
		case REQUEST_CLOSE_ROOM_CODE:
		{
			result = this->closeRoom(requestInfo);
			break;
		}

		case REQUEST_START_GAME_CODE:
		{
			result = this->startGame(requestInfo, lockHandlerFactory);
			break;
		}
		case REQUEST_GET_ROOMS_CODE:
		{
			result = this->getRooms(requestInfo, lockHandlerFactory);
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

// get the room state
RequestResult RoomAdminRequestHandler::getRoomState(const RequestInfo& requestInfo) const
{
	// set
	RequestResult result;
	// set the newHndler to stay on room admin
	result.newHandler = this->m_handlerFactory.createRoomAdminRequestHandler(m_user, m_room);
	// set the response to be the room ID
	result.response = { (unsigned char)(m_room.getRoomData().id) };

	return result;
}


// get all the rooms that exist
RequestResult RoomAdminRequestHandler::getRooms(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// get the rooms as vector of bytes
	std::vector<BYTE> roomsInBytes = RoomHelper::getRoomsInBytes(m_handlerFactory, lockHandlerFactory);

	// set
	RequestResult result;
	// set the newHndler to stay in room admin
	result.newHandler = this->m_handlerFactory.createRoomAdminRequestHandler(m_user, m_room);

	//set the response to be personal stats
	result.response = roomsInBytes;

	return result;
}

// close the room
RequestResult RoomAdminRequestHandler::closeRoom(const RequestInfo& requestInfo)
{
	// set
	RequestResult result;

	// set the room ID in the reault response
	for (char digit : std::to_string(m_room.getRoomData().id))
	{
		result.response.push_back(digit);
	}
	result.response.push_back(',');

	// set all the usernames of the players in the room - accept the admin
	for (std::string username : m_room.getAllUsersAsStrings())
	{
		if (username != m_user.getUsername())
		{
			for (char letter : username)
			{
				result.response.push_back(letter);
			}
			result.response.push_back(',');
		}
	}

	// set the newHndler to be room admin
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(m_user);

	return result;
}

// start the game
RequestResult RoomAdminRequestHandler::startGame(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set that the game begun
	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	m_roomManager.getRoom(m_room.getRoomData().id).setRoomState(GAME_BUGUN);
	handlerFactoryLock.unlock();

	// set
	RequestResult result;

	// set the room ID in the reault response
	for (char number : std::to_string(m_room.getRoomData().id))
	{
		result.response.push_back(number);
	}
	result.response.push_back(',');
	// set all the usernames of the players in the room - accept the admin
	for (std::string username : m_room.getAllUsersAsStrings())
	{
		if (username != m_user.getUsername())
		{
			for (char letter : username)
			{
				result.response.push_back(letter);
			}
			result.response.push_back(',');
		}
	}

	// create new game
	m_handlerFactory.getGameManager().createGame(m_room);

	// set the newHndler to game 
	result.newHandler = this->m_handlerFactory.createGameRequestHandler(m_user);

	return result;
}

