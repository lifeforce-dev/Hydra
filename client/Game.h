//---------------------------------------------------------------
//
// Game.h
//

#pragma once


#include "common/ThreadSafeQueue.h"

#include <memory>
#include <functional>
#include <deque>

namespace sf {
	class RenderWindow;
}

namespace Client {

//===============================================================================

class GameClient;
class GameController;
class Game
{

public:
	Game();
	~Game();

	void Run();
	void PostToMainThread(const std::function<void()>& cb);

	// TODO: 
	// GetMainWindow()

private:
	void ConnectToServer();
	void ProcessCallbackQueue();

private:
	// Functions that need to get processed on the main thread get pushed here.
	Common::ThreadSafeQueue<std::function<void()>> m_callbackQueue;

	// Pull work from the thread safe cb q into here for processing.
	std::deque<std::function<void()>> m_processCbQueue;

	// Don't reorder these.
	// TODO:
	// unique_ptr SDL window, w/e that means
	std::unique_ptr<GameController> m_gameController;

	std::unique_ptr<GameClient> m_client;
};

//===============================================================================

} // namespace Client
