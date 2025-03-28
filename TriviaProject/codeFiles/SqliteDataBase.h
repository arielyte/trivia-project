#pragma once
#include "IDataBase.h"
#include "sqlite3.h"
#include <iostream>
#include <string>
#include <mutex>
#include <algorithm>

#define ONE_GAME 1
#define POINTS_OF_ANSWER 10
#define ID_QUESTION_DEVIDE_EXACTLY 0 // the ID of a question that divide the num of question exactly and turn the modulu to be 0
#define HIGEST_THREE_SCORES 3


class SqliteDataBase : public IDataBase 
{
public:	

	// methods
	virtual bool open() override;
	virtual bool close() override;
	virtual int doesUserExist(const std::string& userName, const std::string& tableTosearchIn) override;
	virtual int doesPasswordMatch(std::string userName, std::string givenPassword) override;
	virtual int addNewUser(std::string userName, std::string userPassword, std::string userEmail) override;
	virtual int submitGameStatistics(GameData& gameData, const std::string& userName) override;
	virtual int addNewUserToStatistics(const std::string& userName) override;

	virtual std::list<Question> getQuestions(const int& amount) override; // Gets the questions with given ID
	virtual float getPlayerAverageAnswerTime(std::string userName) override;
	virtual int getNumOfCorrectAnswers(std::string userName) override; // Gets the number of correct answers the user got
	virtual int getNumOfTotalAnswers(std::string userName) override;
	virtual int getNumOfPlayerGames(std::string userName) override;
	virtual int getPlayerScore(std::string userName) override;
	virtual std::vector<int> getHighScores() override; // Gets the high scores


	// Setter functions
	virtual float setPlayerAverageAnswerTime(float averageAnswerTime, std::string userName) override;
	virtual void setNumOfCorrectAnswers(int numOfCorrectAnswers, std::string userName) override; // Reset this user's stat to 0 when new game is launched
	virtual void setNumOfTotalAnswers(int numOfTotalAnswers, std::string userName) override; // Reset this user's stat to 0 when new game is launched
	virtual void setNumOfPlayerGames(int numOfPlayerGames, std::string userName) override;
	virtual void setPlayerScore(int playerScore, std::string userName) override;

private:
	int res;
	sqlite3* db;
	std::string dbFileName = "TriviaDB";


	// methods
	int getTotalNumOfQuestion();

	// callback methods
	static int callbackDoesMatch(void* data, int argc, char** argv, char** azColName);

	static int callbackGetQuestion(void* data, int argc, char** argv, char** azColName);
	static int callbackGetFloat(void* data, int argc, char** argv, char** azColName);
	static int callbackGetInt(void* data, int argc, char** argv, char** azColName);
	static int callbackGetHS(void* data, int argc, char** argv, char** azColName);
};