//---------------------------------------------------------------
//
// GameClient.cpp
//

#include "server/GameClient.h"

#include "server/Game.h"
#include "server/GameServer.h"

namespace Server {

//===============================================================================

GameClient::GameClient(Game* game, uint32_t clientId)
	: m_game(game)
	, m_clientId(clientId)
{
}

GameClient::~GameClient()
{

}

void GameClient::Process()
{
	// NYI
}

//===============================================================================

} // namespace Server
