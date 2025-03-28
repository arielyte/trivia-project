#pragma once
#include "json.hpp"
// forward declaration for ReqestResult
class IRequestHandler;
#include "HandlerStructs.h"



#define WHOLE_PLAYERS_STORED_IN_THIS_INDEX 0
#define WHOLE_STATS_STORED_IN_THIS_INDEX 0
#define WHOLE_SCORES_STORED_IN_THIS_INDEX 0
#define ONE_COMMA 1
#define ONE_ROOM 1

#define DATA_LENGTH_IN_BYTES 4
#define FIRST_BYTE 0
#define SECOND_BYTE 1
#define THIRD_BYTE 2
#define FOURTH_BYTE 3




// static class
class JsonResponsePacketSerializer 
{
public:
	// the message structure iS:  1 BYTE: MESSAGE CODE, 4 BYTES: MESSAGE SIZE, X BYTES: MESSAGE CONVERTED TO BITS (THEN CALCULATE SIZE) - ALL RETURNED IN A STD::VECTOR OF BYTES  
    // overload methods
    static std::vector<unsigned char> serializeResponseError(const ErrorResponse& error);
	static std::vector<unsigned char> serializeResponse(const GetRoomsResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetHighScoreResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetPlayersInRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetPersonalStatsResponse& response);
	static std::vector<unsigned char> serializeResponse(const LoginResponse& response);
	static std::vector<unsigned char> serializeResponse(const SignupResponse& response);
	static std::vector<unsigned char> serializeResponse(const LogoutResponse& response);
	static std::vector<unsigned char> serializeResponse(const CreateRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const JoinRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const CloseRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const StartGameResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetRoomStateResponse& response);
	static std::vector<unsigned char> serializeResponse(const LeaveRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const LeaveGameResponse& response);
	static std::vector<unsigned char> serializeResponse(GetGameResultsResponse& response);
	static std::vector<unsigned char> serializeResponse(const SubmitAnswerRespone& response);
	static std::vector<unsigned char> serializeResponse(const GetQuestionResponse& response);
};