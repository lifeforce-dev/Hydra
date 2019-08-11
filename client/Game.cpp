//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "client/GameController.h"
#include "client/NetworkController.h"

namespace Client {

//===============================================================================

Game::Game()
	: m_mainWindow(std::make_unique<sf::RenderWindow>(sf::VideoMode(640, 480), "Hydra"))
	, m_networkController(std::make_unique<NetworkController>(this))
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


void Game::ProcessCallbackQueue()
{
	m_callbackQueue.SwapWithEmpty(m_processCbQueue);

	for (auto cb : m_processCbQueue)
	{
		if (cb)
		{
			cb();
		}
	}

	if (!m_processCbQueue.empty())
	{
		std::deque<std::function<void()>>().swap(m_processCbQueue);
	}
}

void Game::Run()
{
	ConnectToServer();

	while (m_mainWindow->isOpen())
	{
		ProcessCallbackQueue();
		m_gameController->Run();
	}
}

void Game::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

//===============================================================================

} // namespace Game

