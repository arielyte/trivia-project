#include "StatisticsManager.h"

StatisticsManager::StatisticsManager()
{
    m_database = new SqliteDataBase;
}
StatisticsManager::~StatisticsManager()
{
    delete m_database;
}

std::vector<int> StatisticsManager::getHighScore()
{
    m_database->open();

	std::vector<int> highScores;
	highScores = m_database->getHighScores();

    m_database->close();


    return highScores;
}

// STATISTIC ORDER: TOTAL GAMES PLAYED - AVERAGE_ANSWER_TIME - NUM_CORRECT_ANSWERS - NUM_TOTAL_ANSWERS - PLAYER_SCORE
std::vector<std::string> StatisticsManager::getUserStatistics(std::string username) 
{
    m_database->open();

    std::vector<std::string> userStats;
    userStats.push_back(std::to_string(m_database->getNumOfPlayerGames(username)));
    userStats.push_back(std::to_string(m_database->getPlayerAverageAnswerTime(username)));
    userStats.push_back(std::to_string(m_database->getNumOfCorrectAnswers(username)));
    userStats.push_back(std::to_string(m_database->getNumOfTotalAnswers(username)));
    userStats.push_back(std::to_string(m_database->getPlayerScore(username)));

    m_database->close();

    return userStats;
}


void StatisticsManager::addToUserTotalGames(std::string username) {
    m_database->open();

    m_database->setNumOfPlayerGames(m_database->getNumOfPlayerGames(username) + 1, username);
    m_database->close();

}

void StatisticsManager::addToUserCorrectAnswers(std::string username) {
    m_database->open();

    m_database->setNumOfCorrectAnswers(m_database->getNumOfCorrectAnswers(username) + 1, username);
    m_database->close();

}

void StatisticsManager::addToUserTotalAnswers(std::string username) {
    m_database->open();

    m_database->setNumOfTotalAnswers(m_database->getNumOfTotalAnswers(username) + 1, username);
    m_database->close();

}

void StatisticsManager::updateAverageAnswerTime(int newAverageAnswerTime, std::string username) {
    m_database->open();

    m_database->setPlayerAverageAnswerTime(m_database->getPlayerAverageAnswerTime(username) + 1, username);
    m_database->close();

}

void StatisticsManager::addToUserPlayerScore(std::string username) {
    m_database->open();

    m_database->setPlayerScore(m_database->getPlayerScore(username) + 10, username);
    m_database->close();

}

void StatisticsManager::resetUserTotalAnswers(std::string username) {
    m_database->open();

    m_database->setNumOfTotalAnswers(0, username);
    m_database->close();

}

void StatisticsManager::resetUserCorrectAnswers(std::string username) {
    m_database->open();

    m_database->setNumOfCorrectAnswers(0, username);
    m_database->close();

}