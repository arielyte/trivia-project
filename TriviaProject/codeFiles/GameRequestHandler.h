#pragma once
#include "IRequestHandler.h"

#ifndef INDEX_OF_FAILES_OR_SUCCED
#define INDEX_OF_FAILES_OR_SUCCED 0
#endif // !INDEX_OF_FAILES_OR_SUCCED

#define EMPTY 0


class GameRequestHandler : public IRequestHandler
{
public:
	//c'tor
	GameRequestHandler(const LoggedUser& user, RequestHandlerFactory& handlerFactoryOfAllPlayers, Game& game);

	//methods
	virtual RequestResult handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory) override;


private:
	// fields
	Game& m_game;
	LoggedUser m_user;
	GameManager& m_gameManger;
	RequestHandlerFactory& m_handlerFactory;


	// helper methods
	RequestResult getQuestion(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory) const;
	RequestResult submitAnswer(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
	RequestResult getGameResults(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory) const;
	RequestResult leaveGame(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory);
};