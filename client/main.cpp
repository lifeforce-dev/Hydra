//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "Game.h"

int main(int argc, char* argv[])
{
	// Init directories.
	if (!is_regular_file(Log::GetLogFile()))
	{
		// Create log file if it doesn't exist
		create_directory("../logs/");
		std::ofstream(Log::GetLogFile().c_str());
	}

	if (!is_regular_file(Log::GetTestsLogFile()))
	{
		// Create log directory.
		create_directory("../logs/tests/");
		std::ofstream(Log::GetTestsLogFile().c_str());
	}

	Client::Game game;
	if (game.Init())
	{
		game.Run();
	}

	return 0;
}
