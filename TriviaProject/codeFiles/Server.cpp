#include "Server.h"

// c'tor
Server::Server()
{
	m_database = new SqliteDataBase;
}
//d'tor
Server::~Server()
{
	delete m_database;
}


// run the communicator and waits to a signal to exit
void Server::run()
{
	// set
	std::string command = "";

	// run the communicator to start listen and accept clients
	std::thread handle(&Communicator::startHandleRequests, m_communicator);
	// detach the thread so it will work without depannd in this function
	handle.detach();

	// wait for signal to exit the program
	while (command != EXIT)
	{
		std::cin >> command;
	}
}

