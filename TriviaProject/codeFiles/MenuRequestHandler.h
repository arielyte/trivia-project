#pragma once
#include "IRequestHandler.h"

#ifndef COMMA_AND_SPACE
#define ONE_COMMA 2
#endif // !COMMA_AND_SPACE





class MenuRequestHandler : public IRequestHandler
{
public:
	//c'tor
	MenuRequestHandler(const LoggedUser& user, RequestHandlerFactory& handlerFactoryOfAllPlayers);

	// methods
	virtual RequestResult handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory) override;

private:
	//fields 
	RequestHandlerFactory& m_handlerFactory;
	LoggedUser m_user;

	// helper methods:
	RequestResult logout(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory) const;
	RequestResult joinRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	RequestResult createRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	// handle request of getts
	RequestResult getRooms(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	RequestResult getPlayersInRoom(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	RequestResult getPersonalStats(const RequestInfo& requestInfo);
	RequestResult getHighScore(const RequestInfo& requestInfo);
};