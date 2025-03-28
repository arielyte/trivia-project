#pragma once
// forward declaration for ReqestResult
class IRequestHandler;
#include "HandlerStructs.h"
#include "RequestHandlerFactory.h"

class RoomHelper
{
public:
	static std::vector<BYTE> getRoomsInBytes(RequestHandlerFactory& handlerFactory, std::mutex& lockHandlerFactory);
};