#pragma once
#include "Question.h"
#include <string>
#include <list>

#define TABLE_USERS "USERS"
#define TABLE_STATISTICS "STATISTICS"


// forward declaration
typedef struct GameData GameData;

class IDataBase 
{
public:
	// D'tor
	virtual ~IDataBase() = default;

	// Methods
	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual int doesUserExist(const std::string& userName, const std::string& tableTosearchIn) = 0;
	virtual int doesPasswordMatch(std::string, std::string) = 0;
	virtual int addNewUser(std::string, std::string, std::string) = 0;
	virtual std::list<Question> getQuestions(const int& amount) = 0; // Gets the questions with given ID
	virtual float getPlayerAverageAnswerTime(std::string) = 0;
	virtual int getNumOfCorrectAnswers(std::string) = 0; // Gets the number of correct answers the user got
	virtual int getNumOfTotalAnswers(std::string) = 0;
	virtual int getNumOfPlayerGames(std::string) = 0;
	virtual int getPlayerScore(std::string) = 0;
	virtual std::vector<int> getHighScores() = 0; // Gets the high scores
	virtual int submitGameStatistics(GameData& gameData, const std::string& userName) = 0;
	virtual int addNewUserToStatistics(const std::string& userName) = 0;

	
	// Setter functions
	virtual float setPlayerAverageAnswerTime(float, std::string) = 0;
	virtual void setNumOfCorrectAnswers(int, std::string) = 0; // Reset this user's stat to 0 when new game is launched
	virtual void setNumOfTotalAnswers(int, std::string) = 0; // Reset this user's stat to 0 when new game is launched
	virtual void setNumOfPlayerGames(int, std::string) = 0;
	virtual void setPlayerScore(int, std::string) = 0;

};