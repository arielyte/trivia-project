#include "Game.h"


// get a quation to ask the player
Question& Game::getQuestionForUser(const LoggedUser& user)
{
	// get the question the user answerd right now
	GameData& gameData = m_players.find(user)->second;
	Question lastQuestion = gameData.currentQuestion;

	// get the next question (the question that comes after the one that the user answerd)
	for (auto iterToQuestion = m_questions.begin(); iterToQuestion != m_questions.end(); iterToQuestion++)
	{
		// if the current question is the one the user answerd
		if (iterToQuestion->getQuestion() == lastQuestion.getQuestion())
		{
			// return the next one
			if (iterToQuestion + NEXT != m_questions.end())
			{
				gameData.currentQuestion = *(iterToQuestion + NEXT);
				return *(iterToQuestion + NEXT);
			}
		}
	}

	// if there is no questions. throw it
	throw std::exception("run out of questions");
}

// update game data for the answer that submited
void Game::submitAnswer(const LoggedUser& user, const unsigned int answerId, const unsigned int answerTimeSeconds)
{
	// get the game data of the player who submited the answer
	GameData& gamedataOfPlayer = m_players.find(user)->second;

	// check if the answer of the player is correct. update as well
	if (gamedataOfPlayer.currentQuestion.getCorrectAnswerId() == answerId)
	{
		++gamedataOfPlayer.correctAnswerCount;
	}
	else
	{
		++gamedataOfPlayer.wrongAnswerCount;
	}

	// update the sum of secondns of all the answers
	gamedataOfPlayer.averageAnswerTime += answerTimeSeconds;
}

// remove player from the room
void Game::removePlayer(const LoggedUser& userToRemove) 
{
	m_players.erase(userToRemove);
}

// get the game results
std::vector<unsigned char> Game::getGameResults(IDataBase* dataBase) const
{
	// set 
	int numOfAnswerdQuestions = 0;
	std::vector<unsigned char> gameResults;

	// get over all the players
	for (std::pair<LoggedUser, GameData> player : m_players)
	{
		// set the player name
		for (char letter : player.first.getUsername())
		{
			gameResults.push_back(letter);
		}
		gameResults.push_back(',');

		// find the num of questions the player answerd
		numOfAnswerdQuestions = getNumOfAnswerdQuestion(player.second.currentQuestion.getQuestion());
		player.second.averageAnswerTime /= numOfAnswerdQuestions;

		// set the score of the player
		gameResults.push_back(player.second.averageAnswerTime);
		gameResults.push_back(',');
		gameResults.push_back(player.second.correctAnswerCount);
		gameResults.push_back(',');
		gameResults.push_back(player.second.wrongAnswerCount);
		gameResults.push_back('.');

		// set the game date into the DB
		submitGameStatsToDB(player.first.getUsername(), player.second, dataBase);
	}

	return gameResults;
}

// find the num of question that the player answerd on
int Game::getNumOfAnswerdQuestion(const std::string& lastAnswerdQuestion) const
{
	// set
	int numOfAnswerdQuestions = 0;

	// get over all the questions
	for (Question question : m_questions)
	{
		if (question.getQuestion() == lastAnswerdQuestion)
		{
			return numOfAnswerdQuestions;
		}
		++numOfAnswerdQuestions;
	}

	return numOfAnswerdQuestions;
}

// check if the game ended
bool Game::isEnded() const
{
	// go over all the players and check if all the players end the game
	for (std::pair< LoggedUser, GameData> player : m_players)
	{
		// check if the player answer in total on number of questions, as the nu,ber of question that exist
		if (player.second.correctAnswerCount + player.second.wrongAnswerCount != m_questions.size() - ONE_DEFAULT_QUESTION)
		{
			// if he did't answer all the question its mean that the game didn't end
			return false;
		}
	}

	// if all the players answerd all the question - its mean that the game ended
	return true;
}


// set the game stats into the database
void Game::submitGameStatsToDB(const std::string& username, GameData& gameStats, IDataBase* dataBase) const
{
	dataBase->open();

	dataBase->submitGameStatistics(gameStats, username);

	dataBase->close();
}


void Game::setQuestions(const std::vector<Question>& questions)
{
	// set
	Question deafoultQuestion;
	deafoultQuestion.setQuestion(FIRST_AND_DEFAULT_QUESTION);

	// set the vector of strings
	m_questions = questions;

	// insert in the begginning an empty string - which is the first question (deafault)
	m_questions.insert(m_questions.begin(), deafoultQuestion);
}
void Game::setPlayers(const std::map<LoggedUser, GameData>& players)
{
	m_players = players;
}
void Game::setGameId(const int ID)
{
	m_gameId = ID;
}




// getter for the game Players
std::map<LoggedUser, GameData>& Game::getPlayers()
{
	return m_players;
}
// getter for the game id
int Game::getId() const
{
	return m_gameId;
}
