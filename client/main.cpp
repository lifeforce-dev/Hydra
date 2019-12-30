//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "Game.h"

#include <filesystem>

int main(int argc, char* argv[])
{
	// Init directories.
	if (!std::filesystem::is_regular_file(Log::GetLogFile()))
	{
		// Create log file if it doesn't exist
		std::filesystem::create_directory("../logs/");
		std::ofstream(Log::GetLogFile().c_str());
	}

	if (!std::filesystem::is_regular_file(Log::GetTestsLogFile()))
	{
		// Create log directory.
		std::filesystem::create_directory("../logs/tests/");
		std::ofstream(Log::GetTestsLogFile().c_str());
	}

	Client::Game game;
	if (game.Initialize())
	{
		game.Run();
	}

	return 0;
}
