//---------------------------------------------------------------
//
// GameController.h
//

#pragma once

#include <memory>

namespace sf {
	class RenderWindow;
}

namespace Client {

//===============================================================================

class NetworkController;
class GameScene;

class GameController
{
public:
	GameController(sf::RenderWindow* window);
	~GameController();

	NetworkController* GetNetworkController() { return m_networkController.get(); }
	void Run();

private:
	std::unique_ptr<GameScene> m_gameScene;
	std::unique_ptr<NetworkController> m_networkController;
};

//===============================================================================

} // namespace Client
