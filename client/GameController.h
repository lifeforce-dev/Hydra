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

class Game;
class GameScene;
class NetworkController;

class GameController
{
public:
	GameController(Game* game);
	~GameController();

	void Run();

private:
	void Initialize();
	Game* m_game;
	NetworkController* m_networkController;
	std::unique_ptr<GameScene> m_gameScene;

	bool m_isInitialized = false;
};

//===============================================================================

} // namespace Client
