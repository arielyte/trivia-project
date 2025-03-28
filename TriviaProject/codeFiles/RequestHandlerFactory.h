#pragma once
#include "SqliteDataBase.h"
#include "LoginManager.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameManager.h"

// forward declaration
class LoginRequestHandler;
class MenuRequestHandler;
class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class GameRequestHandler;




class RequestHandlerFactory 
{
public:
	//c'tor
	RequestHandlerFactory();
	//d'tor
	~RequestHandlerFactory();


	// creators of handlers
	LoginRequestHandler* createLoginRequestHandler();
	MenuRequestHandler* createMenuRequestHandler(const LoggedUser& user);
	RoomAdminRequestHandler* createRoomAdminRequestHandler(const LoggedUser& user, Room& room);
	RoomMemberRequestHandler* createRoomMemberRequestHandler(const LoggedUser& user, Room& room);
	GameRequestHandler* createGameRequestHandler(const LoggedUser& user);

	// getters
	LoginManager& getLoginManager();
	RoomManager& getRoomManager();
	StatisticsManager& getStatisticsManager();
	GameManager& getGameManager();
	IDataBase* getDatabase() const;
	

private:
	IDataBase* m_database;
	LoginManager m_loginManager;
	RoomManager m_roomManager;
	StatisticsManager m_statisticsManager;
	GameManager m_gameManager;


	// helper methods
	Game& getUserGame(const LoggedUser& user) ;
};