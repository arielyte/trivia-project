#include "IRequestHandler.h"


// check if a request is relevent to login handler
bool IRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	// check if the request id is relevant
	for (int relevantrequestId : relevantRequestsIds)
	{
		// if it is relevant return true
		if ((int)(requestInfo.RequestId) == relevantrequestId)
		{
			return true;
		}
	}
	
	// if all the relevant requests ids don't match the request id return false
	return false;
}
