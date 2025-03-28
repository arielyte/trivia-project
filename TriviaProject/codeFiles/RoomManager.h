#pragma once
#include "Room.h"
#include <map>
#include <algorithm>

// duoble declaration of FIRST is possible
#ifndef FIRST
#define FIRST 0
#endif // !FIRST

#define LAST_ID 1
#define DIFERENT_BETWEEN_IDS 1
#define NEXT_ID 1
#define FIRST_ROOM_ID 0
#define DIFERENCE_COUNT_INDEX 1 // the diffrence between the counting 1 2 3 4 ...   and the indexing 0 1 2 3 ... the diffence is 1

class RoomManager
{
public:
	// methods
	void createRoom(const LoggedUser& loggedUser, const RoomData& roomData);
	void deleteRoom(int ID);
	unsigned int getIdForRoom() const;

	
	// getters
	unsigned int getRoomState(const int ID);
	std::vector<RoomData> getRooms();
	Room& getRoom(int ID);
	std::map<int, Room>& getMapRooms();

private:
	std::map<int, Room> m_rooms;
};