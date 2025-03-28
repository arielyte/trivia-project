#include "RoomMemberRequestHandler.h"
#include "MenuRequestHandler.h"
#include "RoomHelper.h"

// c'tor
RoomMemberRequestHandler::RoomMemberRequestHandler(const LoggedUser& user, Room& room, RequestHandlerFactory& handlerFactoryOfAllPlayers) : m_handlerFactory(handlerFactoryOfAllPlayers), m_roomManager(handlerFactoryOfAllPlayers.getRoomManager()), m_room(room)
{
	// declare what the client in this state of room member can do
	this->relevantRequestsIds.push_back(REQUEST_GET_ROOM_STATE_CODE);
	this->relevantRequestsIds.push_back(REQUEST_LEAVE_ROOM_CODE);

	// set the user
	this->m_user = user;
}

// handle a request by room member handeling format
RequestResult RoomMemberRequestHandler::handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
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

		case REQUEST_LEAVE_ROOM_CODE:
		{
			result = this->leaveRoom(requestInfo, lockHandlerFactory);
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
RequestResult RoomMemberRequestHandler::getRoomState(const RequestInfo& requestInfo) const
{
	// set
	RequestResult result;
	// set the newHndler to stay on  member
	result.newHandler = this->m_handlerFactory.createRoomMemberRequestHandler(m_user, m_room);
	// set the response to be the room ID
	result.response = { (unsigned char)(m_room.getRoomData().id) };

	return result;
}


// leave room
RequestResult RoomMemberRequestHandler::leaveRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set
	RequestResult result;
	// set the newHndler to be menu
	result.newHandler = m_handlerFactory.createMenuRequestHandler(m_user);

	// leave the room
	std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
	m_roomManager.getRoom(m_room.getRoomData().id).removeUser(m_user);

	return result;
}


