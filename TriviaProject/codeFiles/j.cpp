#include "StatisticsManager.h"

// c'tor
StatisticsManager::StatisticsManager(IDataBase* database)
{
	this->m_database = database;
}

std::vector<std::string> StatisticsManager::getHighScore() const
{
	std::vector<std::string> a;
	return a;
}
std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& username)const
{
	std::vector<std::string> a;
	return a;
}