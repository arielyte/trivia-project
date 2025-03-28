#pragma once
#include <string>
#include <vector>
#include "LoggedUser.h"

#define NOT_ACTIVE 0
#define ACTIVE 1
#define GAME_BUGUN 2
#define GAME_ENDED 3

// struct of room data
typedef struct RoomData
{
	unsigned int id;
	std::string name;
	unsigned int maxPlayers;
	unsigned int numOfQuestionsInGame;
	unsigned int timePerQuestion;
	unsigned int isActive;

} RoomData;

class Room
{
public:
	// methods
	void addUser(const LoggedUser& loggedUser);
	void removeUser(const LoggedUser& loggedUser);
	void removeUser(const std::string& username);

	std::vector<std::string> getAllUsersAsStrings() const;
	RoomData& getRoomData();
	std::vector<LoggedUser>& getUsers();

	// setter
	void setRoomData(const RoomData& roomData);
    void setRoomState(const int roomState);

	
private:
	//fields 
	RoomData m_metadata;
	std::vector<LoggedUser> m_users;

};