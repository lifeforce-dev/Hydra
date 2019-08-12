//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "common/NetworkHelper.h"
#include "server/GameServer.h"

#include <memory>

int main()
{
	Server::GameServer server;
	server.Run();
	return 0;
}