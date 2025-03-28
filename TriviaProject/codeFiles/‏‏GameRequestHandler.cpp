#include "GameRequestHandler.h"
#include "MenuRequestHandler.h"

// global mutexes
std::mutex lockGameManager;
std::mutex lockGame;

//c'tor
GameRequestHandler::GameRequestHandler(const LoggedUser& user, RequestHandlerFactory& handlerFactoryOfAllPlayers, Game& game) : m_handlerFactory(handlerFactoryOfAllPlayers), m_gameManger(handlerFactoryOfAllPlayers.getGameManager()), m_game(game)
{
	// declare what the client in this state of game can do
	this->relevantRequestsIds.push_back(REQUEST_SUBMIT_ANSWER_CODE);
	this->relevantRequestsIds.push_back(REQUEST_LEAVE_GAME_REQUEST_CODE);
	this->relevantRequestsIds.push_back(REQUEST_GET_QUESTION_REQUEST_CODE);
	this->relevantRequestsIds.push_back(REQUEST_GET_GAME_RESULT_REQUEST_CODE);

	m_user = user;
}


// handle a request by game handeling format
RequestResult GameRequestHandler::handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
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
		case REQUEST_SUBMIT_ANSWER_CODE:
		{
			result = this->submitAnswer(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_LEAVE_GAME_REQUEST_CODE:
		{
			result = this->leaveGame(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_GET_QUESTION_REQUEST_CODE:
		{
			result = this->getQuestion(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_GET_GAME_RESULT_REQUEST_CODE:
		{
			result = this->getGameResults(requestInfo, lockHandlerFactory);
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



// get a question to ask the player
RequestResult GameRequestHandler::getQuestion(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory) const
{
	// set
	Question question;
	RequestResult result;

	// set the newHndler to stay on game 
	result.newHandler = this->m_handlerFactory.createGameRequestHandler(m_user);

	try
	{
		std::lock_guard<std::mutex>handlerFactoryLock(lockHandlerFactory);
		std::lock_guard<std::mutex>GameLock(lockGame);
		std::lock_guard<std::mutex>GameManagerLock(lockGameManager);
		
		// magshimim want to work with the game manager. and not throu the game.
		question = this->m_gameManger.getGame(m_game.getId()).getQuestionForUser(m_user);
		
	    // if there is no exception - and there are more questions, set the status of the response to be 1
		result.response.push_back(RESPONSE_OPERATION_WORKED_CODE);
	}
	catch (std::exception& e)
	{
		// if there is an exception - and there are no questions left, set the status of the response to be 0
		result.response.push_back(RESPONSE_OPERATION_FAILED_CODE);

		// set that the game ended if the it is
		if (m_game.isEnded())
		{
			std::lock_guard<std::mutex>handlerFactoryLock(lockHandlerFactory);
			m_handlerFactory.getRoomManager().getRoom(m_game.getId()).setRoomState(GAME_ENDED);
		}
	}

	// set the response of the result to be the question
	
	// set the question string from the first byte and ends with a dot
	for (char letter : question.getQuestion())
	{
		result.response.push_back(letter);
	}
	result.response.push_back('.');

	// set the question's answers strart after the first dot and ends in the end of the response. the answers saparate with commas
	for (std::string answer : question.getPossibleAnswers())
	{
		for (char letter : answer)
		{
			result.response.push_back(letter);
		}
		result.response.push_back(',');
	}

	
	return result;
}

// update the game with the answer of the player
RequestResult GameRequestHandler::submitAnswer(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set
	RequestResult result;
	// set the newHndler to stay on game 
	result.newHandler = this->m_handlerFactory.createGameRequestHandler(m_user);
	// desrialize the message
	SubmitAnswerRequest submitAnswerRequest = JsonResponsePacketDeserializer::deserializeSubmitAnswerRequest(requestInfo.buffer);


	std::lock_guard<std::mutex>handlerFactoryLock(lockHandlerFactory);
	std::lock_guard<std::mutex>GameLock(lockGame);
	std::lock_guard<std::mutex>GameManagerLock(lockGameManager);
	// magshimim want to work with the game manager. and not through the game.
	this->m_gameManger.getGame(m_game.getId()).submitAnswer(m_user, submitAnswerRequest.answerId, submitAnswerRequest.answerTimeSeconds);

	result.response.push_back(ID_OF_THE_ANSWER_THAT_ALLWAYS_CORRECT);

	return result;
}

// get the game results
RequestResult GameRequestHandler::getGameResults(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory) const
{
	// set
	RequestResult result;
	// set the newHndler to stay on game 
	result.newHandler = this->m_handlerFactory.createGameRequestHandler(m_user);

	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	std::unique_lock<std::mutex> GameLock(lockGame);
	std::unique_lock<std::mutex> GameManagerLock(lockGameManager);
	result.response = this->m_gameManger.getGame(m_game.getId()).getGameResults(m_handlerFactory.getDatabase());
	GameLock.unlock();
	GameManagerLock.unlock();
	
	// set the status of the response as need
	if (m_handlerFactory.getRoomManager().getRoom(m_game.getId()).getRoomData().isActive == GAME_ENDED)
	{
		handlerFactoryLock.unlock();
		result.response.insert(result.response.begin(), RESPONSE_OPERATION_WORKED_CODE);
	}
	else
	{
		handlerFactoryLock.unlock();
		// set the response to be only a status (no results)
		std::vector<unsigned char> emptyResponse;
		result.response = emptyResponse;
		result.response.push_back(RESPONSE_OPERATION_FAILED_CODE);
	}

	return result;
}

// leave the game
RequestResult GameRequestHandler::leaveGame(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set
	RequestResult result;

	// set the newHndler to stay on game 
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(m_user);

	std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
	std::unique_lock<std::mutex> GameLock(lockGame);
	std::unique_lock<std::mutex> GameManagerLock(lockGameManager);

	// remove the player from the game. use the game manager, as magshimim wants
	m_gameManger.getGame(m_game.getId()).removePlayer(m_user);

	GameLock.unlock();
	GameManagerLock.unlock();

	// remove the player from the room
	m_handlerFactory.getRoomManager().getRoom(m_game.getId()).removeUser(m_user); // game ID = room ID

	// if the room is empty - remove it
	if (m_handlerFactory.getRoomManager().getRoom(m_game.getId()).getAllUsersAsStrings().size() == EMPTY)
	{
		m_handlerFactory.getRoomManager().deleteRoom(m_game.getId());
		m_gameManger.deleteGame(m_game.getId());
	}
	return result;
}