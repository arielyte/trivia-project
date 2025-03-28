#include "Room.h"

// Add user to the room
void Room::addUser(const LoggedUser& loggedUser)
{
    // check if the room is active
    if (m_metadata.isActive != ACTIVE)
    {
        throw std::exception("The room can not accept you. maybe it is in a game");
    }

    // check if the room can can add a player
    if (this->m_users.size() < this->m_metadata.maxPlayers)
    {
        // add user
        this->m_users.push_back(loggedUser);
    }
    else
    {
        throw std::exception("Max amount of players in room");
    }
}

// Delete user from the room
void Room::removeUser(const LoggedUser& loggedUser)
{
    for (auto it = m_users.begin(); it != m_users.end(); ++it)
    {
        if (*it == loggedUser)
        {
            m_users.erase(it);
            return; // Stop after removing the first occurrence
        }
    }

    // if there is no user - send it to the client
    throw std::exception("The player not in the room");
}

// Delete user from the room by username
void Room::removeUser(const std::string& username)
{
    for (auto it = m_users.begin(); it != m_users.end(); ++it)
    {
        if (it->getUsername() == username)
        {
            m_users.erase(it);
            return; // Stop after removing the first occurrence
        }
    }

    // if there is no user - send it to the client
    throw std::exception("The player not in the room");
}


// Get all of the users and return them in a string vector
std::vector<std::string> Room::getAllUsersAsStrings() const
{
    std::vector<std::string> usernames;
    for (const auto& user : m_users)
    {
        usernames.push_back(user.getUsername());
    }
    return usernames;
}

// getter for room data
RoomData& Room::getRoomData()
{
    return this->m_metadata;
}
// getter for the users in the room
std::vector<LoggedUser>& Room::getUsers()
{
    return m_users;
}


// set the room data
void Room::setRoomData(const RoomData& roomData)
{
    m_metadata.id = roomData.id;
    m_metadata.isActive = roomData.isActive;
    m_metadata.maxPlayers = roomData.maxPlayers;
    m_metadata.name = roomData.name;
    m_metadata.numOfQuestionsInGame = roomData.numOfQuestionsInGame;
    m_metadata.timePerQuestion = roomData.timePerQuestion;
}
void Room::setRoomState(const int roomState)
{
    m_metadata.isActive = roomState;
}