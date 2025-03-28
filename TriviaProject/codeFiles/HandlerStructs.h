#pragma once
#include "Room.h"
#include <ctime>
#include <Windows.h>
#include <map>


#define FIRST 0


#define REQUEST_LOGIN_CODE 101
#define REQUEST_SIGNUP_CODE 102

#define REQUEST_GET_PLAYERS_IN_ROOM_CODE 103
#define REQUEST_JOIN_ROOM_CODE 104
#define REQUEST_CREATE_ROOM_CODE 105
#define REQUEST_GET_ROOMS_CODE 106
#define REQUEST_LOGOUT_CODE 107
#define REQUEST_GET_STATISTICS_CODE 108
#define REQUEST_GET_HIGH_SCORE_CODE 109

#define REQUEST_CLOSE_ROOM_CODE 110
#define REQUEST_GET_ROOM_STATE_CODE 111
#define REQUEST_START_GAME_CODE 112
#define REQUEST_LEAVE_ROOM_CODE 113

#define REQUEST_SUBMIT_ANSWER_CODE 114
#define REQUEST_LEAVE_GAME_REQUEST_CODE 115
#define REQUEST_GET_QUESTION_REQUEST_CODE 116
#define REQUEST_GET_GAME_RESULT_REQUEST_CODE 117


#define RESPONSE_OPERATION_WORKED_CODE 1
#define RESPONSE_OPERATION_FAILED_CODE 0






// struct for represent requests
typedef struct RequestInfo
{
	BYTE RequestId;
	std::time_t receivalTime;
	std::vector<BYTE> buffer;

}RequestInfo;

// struct for represent requests's respones
typedef struct RequestResult
{
	std::vector<BYTE> response;
	IRequestHandler* newHandler;

}RequestResult;

// struct for represnt results of a player
typedef struct PlayerResults
{
	std::string username;
	unsigned int correctAnswerCount;
	unsigned int worngAnswerCount;
	unsigned int averageAnswerTime;

}PlayerResults;





/*
* serialize - responses
*/



typedef struct ErrorResponse
{
	std::string message;
}ErrorResponse;


typedef struct LoginResponse
{
	unsigned int status;
}LoginResponse;


typedef struct SignupResponse
{
	unsigned int status;
}SignupResponse;


typedef struct LogoutResponse
{
	unsigned int status;
}LogoutResponse;


typedef struct CreateRoomResponse
{
	unsigned int status;
}CreateRoomResponse;


typedef struct JoinRoomResponse
{
	unsigned int status;
}JoinRoomResponse;


typedef struct GetRoomsResponse
{
	unsigned int status;
	std::vector<RoomData> rooms;
}GetRoomsResponse;


typedef struct GetPlayersInRoomResponse
{
	unsigned int status;
	std::vector<std::string> players;
}GetPlayersInRoomResponse;


typedef struct GetHighScoreResponse
{
	unsigned int status;
	std::vector<std::string> statistics;
}GetHighScoreResponse;


typedef struct GetPersonalStatsResponse
{
	unsigned int status;
	std::vector<std::string> statistics;
}GetPersonalStatsResponse;


typedef struct CloseRoomResponse
{
	unsigned int status;
}CloseRoomResponse;


typedef struct StartGameResponse
{
	unsigned int status;
}StartGameResponse;


typedef struct GetRoomStateResponse
{
	unsigned int status;
	bool hasGameBegun;
	std::vector<std::string> players;
	unsigned int questionCount;
	unsigned int answerTimeout;
}GetRoomStateResponse;


typedef struct LeaveRoomResponse
{
	unsigned int status;
}LeaveRoomResponse;


typedef struct LeaveGameResponse
{
	unsigned int status;
}LeaveGameResponse;


typedef struct GetQuestionResponse
{
	unsigned int status;
	std::string question;
	std::map<unsigned int, std::string> answers;
}GetQuestionResponse;


typedef struct SubmitAnswerRespone
{
	unsigned int status;
	unsigned int correctAnswerId;
}SubmitAnswerRespone;


typedef struct GetGameResultsResponse
{
	unsigned int status;
	std::vector<PlayerResults> results;
}GetGameResultsResponse;





/*
* deserialize - requests
*/

// CODE WILL BE: 101
typedef struct LoginRequest
{
	std::string username;
	std::string password;
}LoginRequest;

// CODE WILL BE: 102
typedef struct SignupRequest
{
	std::string username;
	std::string password;
	std::string email;
}SignupRequest;

// CODE WILL BE 103
typedef struct GetPlayersInRoomRequest
{
	unsigned int roomID;
}GetPlayersInRoomRequest;

// CODE WILL BE 104
typedef struct JoinRoomRequest
{
	unsigned int roomID;
}JoinRoomRequest;

// CODE WILL BE 105
typedef struct CreateRoomRequest
{
	std::string roomName;
	unsigned int maxUsers;
	unsigned int questionCount;
	unsigned int answerTimeout;
}CreateRoomRequest;

// CODE WILL BE 114
typedef struct SubmitAnswerRequest
{
	unsigned int answerId;
	unsigned int answerTimeSeconds;
}SubmitAnswerRequest;

//// CODE WILL BE 106 - GetRoomsRequest;
//// CODE WILL BE 107 - LogoutRequest;
//// CODE WILL BE 108 - GetStatisticsRequest;
//// CODE WILL BE 109 - GetHighScoreRequest;
//// CODE WILL BE 110 - CloseRoomRequest;
//// CODE WILL BE 111 - GetRoomStateRequest;
//// CODE WILL BE 112 - StartGameRequest;
//// CODE WILL BE 113 - LeaveRoomRequest;
//// CODE WILL BE 115 - LeaveGameRequest;
//// CODE WILL BE 116 - GetQuestionRequest;
//// CODE WILL BE 117 - GetGameResultRequest;


















