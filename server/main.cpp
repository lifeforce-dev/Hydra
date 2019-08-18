//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "server/Game.h"

#include <memory>

int main()
{
	//init directories.
	if (!is_regular_file(Log::GetLogFile()))
	{
		// create log file if it doesn't exist
		create_directory("../logs/");
		std::ofstream(Log::GetLogFile().c_str());
	}

	Server::Game game;
	game.Run();
	return 0;
}