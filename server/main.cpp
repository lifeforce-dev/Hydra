//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "server/Game.h"

#include <filesystem>
#include <memory>

int main()
{
	//init directories.
	if (!std::filesystem::is_regular_file(Log::GetLogFile()))
	{
		// create log file if it doesn't exist
		std::filesystem::create_directory("../logs/");
		std::ofstream(Log::GetLogFile().c_str());
	}

	Server::Game game;
	game.Run();
	return 0;
}