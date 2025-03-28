#pragma once
#include "LoggedUser.h"
#include "SqliteDataBase.h"
#include <vector>

// RequestHandlerFactory is gonna use this class to manage the logins and signups
class LoginManager {
public:
	
	// C'tor
	LoginManager();
	// d'tor
	~LoginManager();

	void signup(std::string userName, std::string userPassword, std::string userEmail);
	void login(std::string userName, std::string userPassword);
	void logout(std::string userName);

	LoggedUser& findUserByName(const std::string& name);

private:
	IDataBase* m_database;
	std::vector<LoggedUser> m_loggedUsers;	
};