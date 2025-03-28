#include "RoomHelper.h"
#include "RoomAdminRequestHandler.h"

// get the rooms in vector of bytes
std::vector<BYTE> RoomHelper::getRoomsInBytes(RequestHandlerFactory& handlerFactory, std::mutex& lockHandlerFactory)
{
	// set
	std::vector<RoomData> rooms;
	std::vector<BYTE> roomsInBytes;

	try
	{
		std::lock_guard<std::mutex> handlerFactoryLock(lockHandlerFactory);
		// get the rooms
		rooms = handlerFactory.getRoomManager().getRooms();
	}
	catch (std::exception& e)
	{
		throw e;
	}


	// convert the personal stats vector to vector of bytes
	for (RoomData roomData : rooms)
	{
		roomsInBytes.push_back(roomData.id);
		roomsInBytes.push_back(',');
		roomsInBytes.push_back(roomData.isActive);
		roomsInBytes.push_back(',');
		roomsInBytes.push_back(roomData.maxPlayers);
		roomsInBytes.push_back(',');

		for (int c : roomData.name)
		{
			roomsInBytes.push_back(c);
		}
		roomsInBytes.push_back(',');

		roomsInBytes.push_back(roomData.numOfQuestionsInGame);
		roomsInBytes.push_back(',');

		roomsInBytes.push_back(roomData.timePerQuestion);


		roomsInBytes.push_back('.');
	}


	return roomsInBytes;
}