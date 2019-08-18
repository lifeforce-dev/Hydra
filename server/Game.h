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
class GameServer;
class NetworkController;
class Game {
public:
	Game();
	~Game();

	void Run();
	void PostToMainThread(const std::function<void()>& cb);

private:
	void ProcessCallbackQueue();

	std::unique_ptr<GameServer> m_server;

	Common::ThreadSafeQueue<std::function<void()>> m_callbackQueue;
	std::deque<std::function<void()>> m_processCbQueue;

	// Turning this off will shut the server down.
	bool m_isRunning = true;
};

//===============================================================================

} // namespace Server
