#include "LoginManager.h"
#include <stdexcept>

// C'tor
LoginManager::LoginManager()
{
    m_database = new SqliteDataBase;
}
// d'tor
LoginManager::~LoginManager()
{
    delete m_database;
}


// find a user by it's name
LoggedUser& LoginManager::findUserByName(const std::string& name)
{
    // get over all the users
    for (LoggedUser& user : m_loggedUsers)
    {
        // if the user found - return it
        if (user.getUsername() == name)
        {
            return user;
        }
    }
}

void LoginManager::signup(std::string userName, std::string userPassword, std::string userEmail)
{
    m_database->open();

    if (m_database->doesUserExist(userName, TABLE_USERS) == 0) {
        // Add user if he doesn't exist
        m_database->addNewUser(userName, userPassword, userEmail);
    }
    else
    {
        // Throw a message as an exception if the user already exists
        throw std::exception("Error: User already exists. Cannot signup.");
    }

    m_database->close();
}

void LoginManager::login(std::string userName, std::string userPassword)
{
    m_database->open();

    // Check if user exists
    if (m_database->doesUserExist(userName, TABLE_USERS)) {
        // Check if his password matches
        if (m_database->doesPasswordMatch(userName, userPassword)) {
            // check that he is not already connected
            for (LoggedUser user : m_loggedUsers) {
                if (user.getUsername() == userName) {
                    throw std::exception("The user already connected.");
                }
            }

            // Create a LoggedUser instance and store the username
            LoggedUser loggedUser;
            loggedUser.setUsername(userName);
            // Add the user to m_loggedUsers
            m_loggedUsers.push_back(loggedUser);
        }
        else
        {
            // Throw a message as an exception if the password doesn't match
            throw std::exception("Error: Incorrect password. Cannot login.");
        }
    }
    else
    {
        // Throw a message as an exception if the user doesn't exist
        throw std::exception("Error: User does not exist. Cannot login.");
    }

    m_database->close();
}

void LoginManager::logout(std::string userName) {
    // Find the user in m_loggedUsers and remove it
    for (auto it = m_loggedUsers.begin(); it != m_loggedUsers.end(); ++it) {
        if (it->getUsername() == userName) {
            m_loggedUsers.erase(it);
            break;
        }
    }
}
