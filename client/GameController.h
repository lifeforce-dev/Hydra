//---------------------------------------------------------------
//
// GameController.h
//

#pragma once

#include <memory>

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

	// TODO: list of game modules
	std::unique_ptr<GameScene> m_gameScene;

	bool m_isInitialized = false;
};

//===============================================================================

} // namespace Client
