#pragma once
#include "MenuRequestHandler.h"


class LoginRequestHandler : public IRequestHandler
{
public:
	// c'tor
	LoginRequestHandler(RequestHandlerFactory& handlerFactoryOfAllPlayers);

	// methods
	virtual RequestResult handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory) override;

private:
	//fields 
	RequestHandlerFactory& m_handlerFactory;
	LoginManager& m_loginManager;
	// helper methods
	RequestResult login(RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	RequestResult signup(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
};