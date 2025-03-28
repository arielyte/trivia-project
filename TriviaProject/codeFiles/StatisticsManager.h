#pragma once
#include "SqliteDataBase.h"
#include <string>
#include <vector>
#include <algorithm>



class StatisticsManager {
public:
	// c'tor
	StatisticsManager();
	// d'tor
	~StatisticsManager();

	std::vector<int> getHighScore();
	
	// STATISTIC ORDER: TOTAL GAMES PLAYED - AVERAGE_ANSWER_TIME - NUM_CORRECT_ANSWERS - NUM_TOTAL_ANSWERS - PLAYER_SCORE
	std::vector<std::string> getUserStatistics(std::string username);

	// Custom functions
	void addToUserTotalGames(std::string username); // Gets user currect amount of played games and adds 1 to it.
	void addToUserCorrectAnswers(std::string username); // Real time updater for amount of correct answers the user got
	void addToUserTotalAnswers(std::string username); // Real time updater for amount of total answers the user has
	void updateAverageAnswerTime(int newAverageAnswerTime, std::string username); // Update the user's answer time
	void addToUserPlayerScore(std::string username); // Adds +10 to user's score
	void resetUserTotalAnswers(std::string username); // Used to reset user's game stats each new game
	void resetUserCorrectAnswers(std::string username);

private:
	IDataBase* m_database;
};