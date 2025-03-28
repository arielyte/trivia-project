#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"


//c'tor
RequestHandlerFactory::RequestHandlerFactory()
{
	m_database = new SqliteDataBase;
}

//d'tor
RequestHandlerFactory::~RequestHandlerFactory()
{
	delete m_database;
}




// creators of handlers
LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler()
{
	LoginRequestHandler* loginHandler = new LoginRequestHandler(*this);
	return loginHandler;
}
MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(const LoggedUser& user) 
{
	MenuRequestHandler* menuHandler = new MenuRequestHandler(user, *this);
	return menuHandler;
}
RoomAdminRequestHandler* RequestHandlerFactory::createRoomAdminRequestHandler(const LoggedUser& user, Room& room) 
{
	RoomAdminRequestHandler* roomAdminHandler = new RoomAdminRequestHandler(user, room, *this);
	return roomAdminHandler;
}
RoomMemberRequestHandler* RequestHandlerFactory::createRoomMemberRequestHandler(const LoggedUser& user, Room& room)
{
	RoomMemberRequestHandler* roomMemberHandler = new RoomMemberRequestHandler(user, room, *this);
	return roomMemberHandler;
}
GameRequestHandler* RequestHandlerFactory::createGameRequestHandler(const LoggedUser& user)
{
	// get the game that the user in it
	Game& game = getUserGame(user);

	GameRequestHandler* gameHandler = new GameRequestHandler(user, *this, game);

	return gameHandler;
}



// getters
IDataBase* RequestHandlerFactory::getDatabase() const
{
	return m_database;
}
LoginManager& RequestHandlerFactory::getLoginManager()
{
	return m_loginManager;
}
RoomManager& RequestHandlerFactory::getRoomManager()
{
	return m_roomManager;
}
StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
	return m_statisticsManager;
}
GameManager& RequestHandlerFactory::getGameManager()
{
	return m_gameManager;
}

// get the player game (the user in a game)
Game& RequestHandlerFactory::getUserGame(const LoggedUser& user) 
{
	// search in wich room the user in
	for (Game& game : m_gameManager.getGames())
	{
		for (std::pair<const LoggedUser, GameData>& player : game.getPlayers()) // by refernce to save
		{
			// if the user in the room - send its ID
			if (player.first == user)
			{
				return game;
			}
		}
	}
}