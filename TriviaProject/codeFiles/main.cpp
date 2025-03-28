#pragma comment (lib, "ws2_32.lib")
#include "WSAInitializer.h"
#include "Server.h"
#include <iostream>
#include <exception>
#include "SqliteDataBase.h"

int main()
{
	WSAInitializer wsaInit;
	Server myServer;
	try
	{
		myServer.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error occured: " << e.what() << std::endl;
	}

	return 0;
}