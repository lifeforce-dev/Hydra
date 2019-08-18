//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "common/ThreadSafeQueue.h"

#include <deque>
#include <functional>
#include <memory>

namespace Server {

//===============================================================================
class GameClient;
class GameServer;
class NetworkController;
class Game {
public:
	Game();
	~Game();

	void Run();
	void PostToMainThread(const std::function<void()>& cb);

	GameServer* GetGameServer() { return m_server.get(); }

private:
	void ProcessCallbackQueue();

	std::unique_ptr<GameServer> m_server;
	std::vector<std::unique_ptr<GameClient>> m_clients;

	Common::ThreadSafeQueue<std::function<void()>> m_callbackQueue;
	std::deque<std::function<void()>> m_processCbQueue;

	// Turning this off will shut the server down.
	bool m_isRunning = true;
};

//===============================================================================

} // namespace Server
