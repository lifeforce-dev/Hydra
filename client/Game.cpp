//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "GameController.h"

namespace Client {

//===============================================================================

Game::Game()
	: m_mainWindow(std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 480), "Hydra"))
	, m_gameController(std::make_unique<GameController>(m_mainWindow.get()))
{
	m_mainWindow->setFramerateLimit(60);
}

Game::~Game()
{

}

void Game::Run()
{
	while (m_mainWindow->isOpen())
	{
		m_gameController->Run();
	}
}

//===============================================================================

} // namespace Game

