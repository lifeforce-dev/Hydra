//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "common/ThreadSafeQueue.h"

#include <memory>
#include <functional>
#include <deque>

namespace sf {
	class RenderWindow;
}

namespace Client {

//===============================================================================
class GameController;
class NetworkController;
class Game
{

public:
	Game();
	~Game();

	void Run();
	void PostToMainThread(const std::function<void()>& cb);

	sf::RenderWindow* GetMainWindow() { return m_mainWindow.get(); }
	NetworkController* GetNetworkController() { return m_networkController.get(); }

private:
	void ConnectToServer();
	void ProcessCallbackQueue();

private:
	// Functions that need to get processed on the main thread get pushed here.
	Common::ThreadSafeQueue<std::function<void()>> m_callbackQueue;

	// Pull work from the thread safe cb q into here for processing.
	std::deque<std::function<void()>> m_processCbQueue;

	// Don't reorder these.
	std::unique_ptr<sf::RenderWindow> m_mainWindow;
	std::unique_ptr<NetworkController> m_networkController;
	std::unique_ptr<GameController> m_gameController;
};

//===============================================================================

} // namespace Client
