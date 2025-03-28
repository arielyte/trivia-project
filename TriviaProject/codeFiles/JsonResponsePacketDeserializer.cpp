#include "JsonResponsePacketDeserializer.h"

// deserialize methos to make buffer into login request
LoginRequest JsonResponsePacketDeserializer::deserializeLoginRequest(const std::vector<unsigned char>& buffer) {
    // set
    std::string jsonMessage(buffer.begin(), buffer.end());

    // Deserialize JSON message into LoginRequest
    auto json = nlohmann::json::parse(jsonMessage);
    LoginRequest request;
    request.username = json["username"];
    request.password = json["password"];

    return request;
}

// deserialize methos to make buffer into signup request
SignupRequest JsonResponsePacketDeserializer::deserializeSignupRequest(const std::vector<unsigned char>& buffer) 
{
    // set
    std::string jsonMessage(buffer.begin(), buffer.end());

    // Deserialize JSON message into SignupRequest
    auto json = nlohmann::json::parse(jsonMessage);
    SignupRequest request;
    request.username = json["username"];
    request.password = json["password"];
    request.email = json["email"];

    return request;
}

// deserialize methos to make buffer into get players in room request
GetPlayersInRoomRequest JsonResponsePacketDeserializer::deserializeGetPlayersInRoomRequest(const std::vector<unsigned char>& buffer) 
{
    // set
    std::string jsonMessage(buffer.begin(), buffer.end());

    // Deserialize JSON message into GetPlayersInRoomRequest 
    auto json = nlohmann::json::parse(jsonMessage);

    GetPlayersInRoomRequest request = { 0 };
    request.roomID = json["RoomID"];

    return request;
}

// deserialize methos to make buffer into create room request
CreateRoomRequest JsonResponsePacketDeserializer::deserializeCreateRoomRequest(const std::vector<unsigned char>& buffer)
{
    // set
    std::string jsonMessage(buffer.begin(), buffer.end());

    // Deserialize JSON message into CreateRoomRequest
    auto json = nlohmann::json::parse(jsonMessage);
    CreateRoomRequest request;
    request.answerTimeout = json["AnswerTimeout"];
    request.maxUsers = json["MaxUsers"];
    request.questionCount = json["QuestionCount"];
    request.roomName = json["RoomName"];

    return request;
}

// deserialize methos to make buffer into join room request
JoinRoomRequest JsonResponsePacketDeserializer::deserializeJoinRoomRequest(const std::vector<unsigned char>& buffer)
{
    // set
    std::string jsonMessage(buffer.begin(), buffer.end());

    // Deserialize JSON message into JoinRoomRequest
    auto json = nlohmann::json::parse(jsonMessage);
    JoinRoomRequest request = { 0 };
    request.roomID = json["RoomID"];

    return request;
}

// deserialize methos to make buffer into join room request
SubmitAnswerRequest JsonResponsePacketDeserializer::deserializeSubmitAnswerRequest(const std::vector<unsigned char>& buffer)
{
    // set
    std::string jsonMessage(buffer.begin(), buffer.end());

    // Deserialize JSON message into JoinRoomRequest
    auto json = nlohmann::json::parse(jsonMessage);
    SubmitAnswerRequest request = { 0 };
    request.answerId = json["AnswerID"];
    request.answerTimeSeconds = json["AnswerTimeSeconds"];

    return request;
}