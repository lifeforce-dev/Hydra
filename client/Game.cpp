//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "GameController.h"
#include "NetworkController.h"

namespace Client {

//===============================================================================

Game::Game()
	: m_mainWindow(std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 480), "Hydra"))
	, m_networkController(std::make_unique<NetworkController>())
	, m_gameController(std::make_unique<GameController>(this))
{
	m_mainWindow->setFramerateLimit(60);
}

Game::~Game()
{

}

void Game::ConnectToServer()
{
	m_networkController->ConnectToServer();

	if (m_networkController->IsConnected())
	{
		LOG_DEBUG("Successfully connected to server.");
	}
}


void Game::Run()
{
	ConnectToServer();

	while (m_mainWindow->isOpen())
	{
		m_gameController->Run();
	}
}

void Game::PostToMainThread(std::function<void()> cb)
{
	// NYI
}

//===============================================================================

} // namespace Game

