//---------------------------------------------------------------
//
// Game.h
//

#pragma once


#include "common/ThreadSafeQueue.h"

#include <memory>
#include <functional>
#include <deque>

namespace Client {

//===============================================================================

class MainWindow;
class GameClient;
class GameController;
class Game
{

public:
	Game();
	~Game();

	// Initialized game systems. Returns true if succesful.
	bool Init();

	// Run the game.
	void Run();

	// Ensures that any callback passed in here gets executed on the main thread.
	void PostToMainThread(const std::function<void()>& cb);

private:
	void ConnectToServer();
	void ProcessCallbackQueue();

private:

	// Functions that need to get processed on the main thread get pushed here.
	Common::ThreadSafeQueue<std::function<void()>> m_callbackQueue;

	// Pull work from the thread safe cb q into here for processing.
	std::deque<std::function<void()>> m_processCbQueue;

	// Don't reorder these.
	std::unique_ptr<GameController> m_gameController;

	std::unique_ptr<GameClient> m_client;

	std::unique_ptr<MainWindow> m_mainWindow;

	bool m_isInitialized = false;
};

//===============================================================================

} // namespace Client
