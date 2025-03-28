#pragma once
#include "json.hpp"
// forward declaration for ReqestResult
class IRequestHandler;
#include "HandlerStructs.h"


class JsonResponsePacketDeserializer
{
public:
	// Request structure: MESSAGE CODE (1 BYTE) - "MESSAGE SIZE" (4 BYTES) - MESSAGE (X BYTES)
	static LoginRequest deserializeLoginRequest(const std::vector<unsigned char>& buffer);
	static SignupRequest deserializeSignupRequest(const std::vector<unsigned char>& buffer);
	static GetPlayersInRoomRequest deserializeGetPlayersInRoomRequest(const std::vector<unsigned char>& buffer);
	static CreateRoomRequest deserializeCreateRoomRequest(const std::vector<unsigned char>& buffer);
	static JoinRoomRequest deserializeJoinRoomRequest(const std::vector<unsigned char>& buffer);
	static SubmitAnswerRequest deserializeSubmitAnswerRequest(const std::vector<unsigned char>& buffer);
};