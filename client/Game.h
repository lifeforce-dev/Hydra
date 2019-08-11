//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>
#include <functional>
#include <queue>

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
	void PostToMainThread(std::function<void()> cb);

	sf::RenderWindow* GetMainWindow() { return m_mainWindow.get(); }
	NetworkController* GetNetworkController() { return m_networkController.get(); }

private:
	void ConnectToServer();

private:

	// Functions that need to get processed on the main thread get pushed here.
	std::vector<std::function<void()>> m_cbQueue;

	// Don't reorder these.
	std::unique_ptr<sf::RenderWindow> m_mainWindow;
	std::unique_ptr<NetworkController> m_networkController;
	std::unique_ptr<GameController> m_gameController;
};

//===============================================================================

} // namespace Client
