#pragma once
#include "Game.h"
#include "SqliteDataBase.h"
#include "Room.h"

class GameManager
{
public:
	//c'tor
	GameManager();
	//d'tor
	~GameManager();
	
	// methods
	Game& createGame(Room& room);
	void deleteGame(const int gameId);
	Game& getGame(const int gameId);

	// getters
	std::vector<Game>& getGames();

private:
	// fields
	IDataBase* m_database;
	std::vector<Game> m_games;

	// helper method
	std::vector<Question> getQuestionsFormDb(const int questionCount) const;
	std::map<LoggedUser, GameData> getPlayers(Room& room) const;
};