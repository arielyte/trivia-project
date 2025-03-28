#pragma once
#include <iostream>
#include <exception>
#include "JsonResponsePacketDeserializer.h"
#include <mutex>
#include "RequestHandlerFactory.h"




class IRequestHandler
{
public:
	// c'tor
	virtual ~IRequestHandler() = default;

	// methods
	virtual bool isRequestRelevant(const RequestInfo& requestInfo);
	virtual RequestResult handleRequest(RequestInfo& requestInfo, std::mutex& lockHandlerFactory) = 0;

protected:
	// fields
	std::vector<int> relevantRequestsIds;
};





