//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "client/GameClient.h"
#include "client/GameController.h"
#include <SDL.h>

namespace Client {

//===============================================================================

namespace {
	const int32_t s_screenHeight = 1920;
	const int32_t s_screenWidth = 1920;

	std::shared_ptr<spdlog::logger> s_logger;
}

//-------------------------------------------------------------------------------

Game::Game()
	: m_gameController(std::make_unique<GameController>(this))
	, m_client(std::make_unique<GameClient>(this))
	//, m_mainWindow(std::make_unique<SDL_Window>())
{
	REGISTER_LOGGER("GameClient");
	s_logger = Log::Logger("GameClient");


	Init();
}

Game::~Game()
{
}

bool Game::Init()
{
	SDL_Window* window = NULL;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("Hydra", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		s_screenHeight, s_screenWidth, SDL_WINDOW_SHOWN);

	if (!window)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create SDL window. error={}", SDL_GetError());
		return false;
	}

	return true;
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

	// TODO: while main window is open.
	while (false)
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

