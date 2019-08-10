//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>

namespace Client {

//===============================================================================

class GameController;
class Game
{

public:
	Game();
	~Game();

	void Run();

private:
	// Don't reorder these.
	std::unique_ptr<sf::RenderWindow> m_mainWindow;
	std::unique_ptr<GameController> m_gameController;

};

//===============================================================================

} // namespace Client
