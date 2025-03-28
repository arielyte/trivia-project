#pragma once
#include "IRequestHandler.h"


#ifndef ONE_COMMA
#define COMMA_AND_SPACE 2
#endif // !COMMA_AND_SPACE

class RoomMemberRequestHandler : public IRequestHandler
{
public:
	// c'tor
	RoomMemberRequestHandler(const LoggedUser& user, Room& room, RequestHandlerFactory& handlerFactoryOfAllPlayers);

	// methods
	virtual RequestResult handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory) override;

private:
	// helper methods
	RequestResult leaveRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	RequestResult getRoomState(const RequestInfo& requestInfo) const;

	// fields
	Room& m_room;
	LoggedUser m_user;
	RoomManager& m_roomManager;
	RequestHandlerFactory& m_handlerFactory;
};