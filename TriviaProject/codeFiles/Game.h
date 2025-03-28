#pragma once
#include "Question.h"
#include <vector>
#include <map>
#include "LoggedUser.h"


#include "SqliteDataBase.h"


#define NEXT 1

#define FIRST_AND_DEFAULT_QUESTION "" // the first question need to be exist, for the sending of the real first question 
#define ONE_DEFAULT_QUESTION 1

typedef struct GameData 
{
	Question currentQuestion;
	unsigned int correctAnswerCount;
	unsigned int wrongAnswerCount;
	unsigned int averageAnswerTime;
}GameData;


class Game
{
public:
	// methods
	Question& getQuestionForUser(const LoggedUser& user);
	void submitAnswer(const LoggedUser& user, const unsigned int answerId, const unsigned int answerTimeSeconds);
	void removePlayer(const LoggedUser& userToRemove);
	std::vector<unsigned char> getGameResults(IDataBase* dataBase) const;
	int getNumOfAnswerdQuestion(const std::string& lastAnswerdQuestion) const;
	bool isEnded() const;

	// getters
	std::map<LoggedUser, GameData>& getPlayers();
	int getId() const;

	// setters
	void setQuestions(const std::vector<Question>& questions);
	void setPlayers(const std::map<LoggedUser, GameData>& players);
	void setGameId(const int ID);
private:
	// fields
	std::vector<Question> m_questions;
	std::map<LoggedUser, GameData> m_players;
	int m_gameId;

	// helper methods
	void submitGameStatsToDB(const std::string& username, GameData& gameStats, IDataBase* dataBase) const;
};