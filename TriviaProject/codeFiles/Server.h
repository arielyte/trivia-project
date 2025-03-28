#pragma once
#include <thread>
#include <exception>
#include <iostream>
#include "Communicator.h"
#include "IDataBase.h"
#include "SqliteDataBase.h"

//TODO: include the requestHandlerFactory

#define EXIT "EXIT"

class Server 
{
public:

	// c'tor
	Server();
	//d'tor
	~Server();

	// methods
	void run();

private:
	// fields
	Communicator m_communicator;
	IDataBase* m_database;
};