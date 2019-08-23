//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "Game.h"

int main()
{
	//init directories.
	if (!is_regular_file(Log::GetLogFile()))
	{
		// create log file if it doesn't exist
		create_directory("../logs/");
		std::ofstream(Log::GetLogFile().c_str());
	}

	Client::Game game;
	if (game.Init())
	{
		game.Run();
	}

	return 0;
}
