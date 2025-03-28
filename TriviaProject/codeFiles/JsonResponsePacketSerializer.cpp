#include "JsonResponsePacketSerializer.h"

// overloaded methods to serilze different (and same) ways different types of responses
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponseError(const ErrorResponse& error)
{
    // Construct JSON string
    std::string stringMessage = "{\"message\": \"" + error.message + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomsResponse& response)
{
    // Convert the rooms vector into string
    std::string roomsString;
    for (size_t i = 0; i < response.rooms.size(); ++i)
    {
        const RoomData& room = response.rooms[i];
        roomsString += "{\"ID\": " + std::to_string(room.id) +
            ", \"IsActive\": " + std::to_string(room.isActive) +
            ", \"MaxPlayers\": " + std::to_string(room.maxPlayers) +
            ", \"Name\": \"" + room.name +
            "\", \"NumberOfQuestions\": " + std::to_string(room.numOfQuestionsInGame) +
            ", \"TimePerQuestion\": " + std::to_string(room.timePerQuestion) +
            "}";

        if (i < response.rooms.size() - 1)
            roomsString += ",";
    }

    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) +
        "\", \"Rooms\": [" + roomsString + "]}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());
    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetHighScoreResponse& response)
{
    // set
    std::string statisticsString = "\"HighScores\": \"" + response.statistics[WHOLE_SCORES_STORED_IN_THIS_INDEX] + "\"";

    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\", " + statisticsString + "}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());
    
    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& response)
{
    // set
    std:: string playersString = "\"Players\": \"" + response.players[WHOLE_PLAYERS_STORED_IN_THIS_INDEX] + "\"";
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"," + playersString + "}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetPersonalStatsResponse& response)
{
    // set
    std::string statisticsString = "\"PersonalStats\": \"" + response.statistics[WHOLE_STATS_STORED_IN_THIS_INDEX] + "\"";

    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\", " + statisticsString + "}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CloseRoomResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const StartGameResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());


    BYTE dataLength[DATA_LENGTH_IN_BYTES] = { 0 };
    // split the data length to four bytes
    dataLength[FIRST_BYTE] = stringMessage.size() & 0xFF;
    dataLength[SECOND_BYTE] = stringMessage.size() >> 8 & 0xFF;
    dataLength[THIRD_BYTE] = stringMessage.size() >> 16 & 0xFF;
    dataLength[FOURTH_BYTE] = stringMessage.size() >> 24 & 0xFF;

    for (int i = FIRST; i < DATA_LENGTH_IN_BYTES; i++)
    {
        // set the data length of the resopnse
        buffer.insert(buffer.begin(), dataLength[i]);
    }
    // set the code massage of the resopnse
    buffer.insert(buffer.begin(), REQUEST_START_GAME_CODE);
   

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomStateResponse& response)
{
    // get the players as string
    std::string stringPlayers = "";
    for (std::string username : response.players)
    {
        stringPlayers += (username + ',');
    }

    // Construct JSON string
    std::string stringMessage = "{\"Status\": " + std::to_string(response.status) + ", \"HasGameBegun\": " + std::to_string(response.hasGameBegun) + ", \"Players\": \"" + stringPlayers + "\", \"QuestionCount\": " + std::to_string(response.questionCount) + ", \"AnswerTimeout\": " + std::to_string(response.answerTimeout) + "}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LeaveRoomResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());


    BYTE dataLength[DATA_LENGTH_IN_BYTES] = { 0 };
    // split the data length to four bytes
    dataLength[FIRST_BYTE] = stringMessage.size() & 0xFF;
    dataLength[SECOND_BYTE] = stringMessage.size() >> 8 & 0xFF;
    dataLength[THIRD_BYTE] = stringMessage.size() >> 16 & 0xFF;
    dataLength[FOURTH_BYTE] = stringMessage.size() >> 24 & 0xFF;

    for (int i = FIRST; i < DATA_LENGTH_IN_BYTES; i++)
    {
        // set the data length of the resopnse
        buffer.insert(buffer.begin(), dataLength[i]);
    }
    // set the code massage of the resopnse
    buffer.insert(buffer.begin(), REQUEST_LEAVE_ROOM_CODE);

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LoginResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SignupResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LeaveGameResponse& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(GetGameResultsResponse& response)
{
    // set
    std::string results = "";

    // set a string that include all the players and their results
    for (PlayerResults& player : response.results)
    {
        // for each player - set a line with his results
        results += player.username + "," + std::to_string(player.correctAnswerCount) + "," + std::to_string(player.worngAnswerCount) + "," + std::to_string(player.averageAnswerTime) + ".";
    }

    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\", " + "\"Results\": \"" + results + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SubmitAnswerRespone& response)
{
    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetQuestionResponse& response)
{
    // set
    std::string answers = "";

    // set a string that include all the answers
    for (std::pair<unsigned int, std::string> answer : response.answers)
    {
        // for each answer - set a line with the answer
        answers += answer.second + ", ";
    }

    // Remove the last comma and space
    if (!answers.empty())
    {
        answers = answers.substr(0, answers.size() - 2);
    }

    // Construct JSON string
    std::string stringMessage = "{\"Status\": \"" + std::to_string(response.status) + "\", " + "\"Question\": \"" + response.question + "\", " + "\"Answers\": \"" + answers + "\"}";

    // Convert string to buffer
    std::vector<unsigned char> buffer(stringMessage.begin(), stringMessage.end());

    return buffer;
}
