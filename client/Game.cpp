//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "client/GameClient.h"
#include "client/GameController.h"
#include "client/MainWindow.h"

namespace Client {

//===============================================================================

namespace {
	std::shared_ptr<spdlog::logger> s_logger;
}

Game* g_game = nullptr;

//-------------------------------------------------------------------------------

Game::Game()

{
	REGISTER_LOGGER("Game");
	s_logger = Log::Logger("Game");
	g_game = this;
}

Game::~Game()
{
	g_game = nullptr;
}

bool Game::Init()
{
	m_renderEngine = std::make_unique<RenderEngine>();
	m_isInitialized = m_renderEngine->Initialize();
	m_mainWindow = m_renderEngine->GetMainWindow();

	// TODO: Pull this from a file with a FontManager.
	m_defaultFont = SDL_FontPtr(std::move(
		TTF_OpenFont("resources/fonts/Charter Regular.ttf", 24)), TTF_CloseFont);

	if (!m_defaultFont)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to load font. error=", TTF_GetError());
		m_isInitialized = false;
	}

	if (!m_isInitialized)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize game. Exiting...");
		m_isInitialized = false;
	}

	m_gameController = std::make_unique<GameController>(this);
	m_client = std::make_unique<GameClient>(this);

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
		m_mainWindow->Process();

		m_gameController->Run();

		m_renderEngine->Render();
	}
}

void Game::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

TTF_Font* Game::GetDefaultFont()
{
	return m_defaultFont.get();
}

Client::RenderEngine* Game::GetRenderEngine() const
{
	return m_renderEngine.get();
}

//===============================================================================

} // namespace Game

