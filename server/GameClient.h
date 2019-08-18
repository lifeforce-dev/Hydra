//---------------------------------------------------------------
//
// GameClient.h
//

#pragma once

#include <cstdint>

namespace Server {

//===============================================================================

class Game;
class GameClient {
public:
	GameClient(Game* game, uint32_t clientId);
	~GameClient();

	// This will process every system needed for game simulation on this client. 
	void Process();

private:
	Game* m_game;
	uint32_t m_clientId = 0;
};

//===============================================================================

} // namespace Server
