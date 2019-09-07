//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "client/GameClient.h"
#include "client/GameController.h"
#include "client/MainWindow.h"
#include "client/RenderEngine.h"

#include <SDL.h>

namespace Client {

//===============================================================================

namespace {
	const int32_t s_screenWidth = 1920;
	const int32_t s_screenHeight = 1080;
	std::shared_ptr<spdlog::logger> s_logger;
}

//-------------------------------------------------------------------------------

Game::Game()
	: m_gameController(std::make_unique<GameController>(this))
	, m_client(std::make_unique<GameClient>(this))
	, m_mainWindow(std::make_unique<MainWindow>())
	, m_renderEngine(std::make_unique<RenderEngine>())
{
	REGISTER_LOGGER("Game");
	s_logger = Log::Logger("Game");
}

Game::~Game()
{
}

bool Game::Init()
{
	m_isInitialized = m_renderEngine->Initialize() && m_mainWindow->Initialize();

	if (!m_isInitialized)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize game. Exiting...");
	}

	return m_isInitialized;
}

void Game::ConnectToServer()
{
	m_client->Start();
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
	while (m_mainWindow->IsOpen())
	{
		ProcessCallbackQueue();
		m_gameController->Run();
		m_mainWindow->Process();
	}
}

void Game::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

//===============================================================================

} // namespace Game

