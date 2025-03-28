#include "LoginRequestHandler.h"


// c'tor
LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory& handlerFactoryOfAllPlayers) : m_handlerFactory(handlerFactoryOfAllPlayers), m_loginManager(handlerFactoryOfAllPlayers.getLoginManager())
{
	// declare what the client in this state of login can do
	this->relevantRequestsIds.push_back(REQUEST_LOGIN_CODE);
	this->relevantRequestsIds.push_back(REQUEST_SIGNUP_CODE);
}



// handle a request by login handeling format
RequestResult LoginRequestHandler::handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
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
		// check if the request is login or signup. And active the right method to treat the request
		switch (requestInfo.RequestId)
		{
		case REQUEST_LOGIN_CODE:
		{
			result = login(requestInfo, lockHandlerFactory);
			break;
		}

		case REQUEST_SIGNUP_CODE:
		{
			result = signup(requestInfo, lockHandlerFactory);
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

// login a client
RequestResult LoginRequestHandler::login(RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// set
	RequestResult result;

	// desrialize the message
	LoginRequest loginRequest = JsonResponsePacketDeserializer::deserializeLoginRequest(requestInfo.buffer);

	try 
	{
		// Take care of the client in the login manager
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		m_loginManager.login(loginRequest.username, loginRequest.password);
	}
	catch (std::exception& e)
	{
		// set the handler to login state after we deleting the menu handler
		//delete result.newHandler;
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		result.newHandler = m_handlerFactory.createLoginRequestHandler();
		throw e;
	}
	
	// set the handler to menu state
	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	result.newHandler = m_handlerFactory.createMenuRequestHandler(m_loginManager.findUserByName(loginRequest.username));
	handlerFactoryLock.unlock();

	for (int i = FIRST; i < loginRequest.username.size(); i++)
	{
		result.response.push_back(loginRequest.username[i]);
	}

	return result; 
}

// Signup a client
RequestResult LoginRequestHandler::signup(const RequestInfo& requestInfo, std::mutex& lockHandlerFactory)
{
	// Set
	RequestResult result;

	// Set the handler to stay in login state
	std::unique_lock<std::mutex> handlerFactoryLock(lockHandlerFactory);
	result.newHandler = m_handlerFactory.createLoginRequestHandler();
	handlerFactoryLock.unlock();

	// desrialize the message
	SignupRequest signupRequest = JsonResponsePacketDeserializer::deserializeSignupRequest(requestInfo.buffer);
	
	try {
		// Take care of the client in the signup manager
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		m_loginManager.signup(signupRequest.username, signupRequest.password, signupRequest.email);
	}
	catch (std::exception& e) 
	{
		throw e;
	}

	return result;
}