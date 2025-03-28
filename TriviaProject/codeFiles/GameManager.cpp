#include "GameManager.h"



//c'tor
GameManager::GameManager()
{
	m_database = new SqliteDataBase;
}
//d'tor
GameManager::~GameManager()
{
	delete m_database;
}


// create room
Game& GameManager::createGame(Room& room) 
{
	// set
	Game newGame;

	// sets of the game
	newGame.setGameId(room.getRoomData().id);
	newGame.setPlayers(getPlayers(room));
	newGame.setQuestions(getQuestionsFormDb(room.getRoomData().numOfQuestionsInGame));

	m_games.push_back(newGame);

	return newGame;
}

// delete room
void GameManager::deleteGame(const int gameId) 
{
	try
	{
		// get over all the games and delete them
		for (auto it = m_games.begin(); it != m_games.end(); it++)
		{
			if (it->getId() == gameId)
			{
				m_games.erase(it); \
				break;
			}
		}
	}
	catch (...)
	{
		throw std::exception("Can't find the game");
	}
}

// getter for map of games and their IDs
std::vector<Game>& GameManager::getGames()
{
	return m_games;
}

// return agame by its ID
Game& GameManager::getGame(const int gameId)
{
	// get over all the games
	for (Game& game : m_games)
	{
		// if the game is the game that we are looking for, return it
		if (game.getId() == gameId)
		{
			return game;
		}
	}
}

// get from room the players and return them
std::map<LoggedUser, GameData> GameManager::getPlayers(Room& room) const
{
	// set
	Question defaultQuestion;
	defaultQuestion.setQuestion("");
	GameData defaultGameData{ defaultQuestion,0,0,0 };
	std::map <LoggedUser, GameData> players;

	// set the players of the game
	for (LoggedUser& player : room.getUsers())
	{
		players.insert({ player, defaultGameData });
	}

	return players;
}

// get from database questions
std::vector<Question> GameManager::getQuestionsFormDb(const int questionCount) const
{
	// get questions from database
	m_database->open();
	std::list<Question> questionsList = m_database->getQuestions(questionCount);
	m_database->close();

	// convert the list to vector
	std::vector<Question> questionsVector{ std::begin(questionsList), std::end(questionsList) };

	return questionsVector;
}