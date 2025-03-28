#include "RoomManager.h"

// get a free ID for a room
unsigned int RoomManager::getIdForRoom() const
{
	// set
	std::vector<unsigned int> roomIds;

	// if this is the first room set its ID to 0
	if (!m_rooms.size())
	{
		return FIRST_ROOM_ID;
	}

	// get over all the room IDs into a vector
	for (std::pair<int, Room> room: m_rooms)
	{
		roomIds.push_back(room.first);
	}

	// sort all the IDs
	std::sort(roomIds.begin(), roomIds.end());
	
	// check if the first room stopped working
	if (roomIds[FIRST] != FIRST_ROOM_ID)
	{
		// set the next room get this ID
		return FIRST_ROOM_ID;
	}

	// find the first ID that is not occupied
	for (int i = FIRST; i < roomIds.size() - LAST_ID; i++)
	{
		// if the next ID in the vector is not what it should be - it is OK to use it (a room with this ID stopped)
		if (roomIds[i] + DIFERENT_BETWEEN_IDS != roomIds[i + NEXT_ID])
		{
			return roomIds[i] + DIFERENT_BETWEEN_IDS;
		}
	}

	// if it went over all the IDs untill the last one, and all was occupied
	// set the room ID to be the next ID after the last one
	return roomIds[roomIds.size() - DIFERENCE_COUNT_INDEX] + DIFERENT_BETWEEN_IDS;
}


// Implementation of createRoom function
void RoomManager::createRoom(const LoggedUser& loggedUser, const RoomData& roomData)
{
	try
	{
		Room newRoom;
		newRoom.setRoomData(roomData);
		newRoom.addUser(loggedUser);
		m_rooms.insert({ roomData.id, newRoom });
	}
	catch (...)
	{
		throw std::exception("Can't create the room");
	}
}

// Implementation of deleteRoom function
void RoomManager::deleteRoom(const int ID)
{
	try
	{
		//delete& m_rooms.find(ID)->second;
		m_rooms.erase(ID);
	}
	catch (...)
	{
		throw std::exception("Can't find the room");
	}
}

// getter
std::map<int, Room>& RoomManager::getMapRooms()
{
	return this->m_rooms;
}

// Implementation of getRoomState function
unsigned int RoomManager::getRoomState(const int ID) 
{
	auto it = m_rooms.find(ID);
	if (it != m_rooms.end())
	{
		return it->second.getRoomData().isActive;
	}
	else
	{
		throw std::exception("C'ant find the room");
	}
	// Return NOT_ACTIVE if room with given ID is not found
	return NOT_ACTIVE;
}

// Implementation of getRooms function
std::vector<RoomData> RoomManager::getRooms() 
{
	// set
	std::vector<RoomData> roomsData;

	try
	{
		for (auto& pair : m_rooms)
		{
			roomsData.push_back(pair.second.getRoomData());
		}
	}
	catch (...)
	{
		throw std::exception("Can't get the rooms in the game");
	}

	return roomsData;
}

// Implementation of getRoom function
Room& RoomManager::getRoom(const int ID) 
{
	auto room = m_rooms.find(ID);
	
	if (room == m_rooms.end())
	{
		throw std::exception("Can't find the room");
	}
	return room->second;
}
