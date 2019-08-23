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
	const int32_t s_screenWidth = 1920;
	const int32_t s_screenHeight = 1080;

	std::shared_ptr<spdlog::logger> s_logger;
}

class MainWindow {
public:

	MainWindow()
	{
	}

	~MainWindow()
	{
		Close();
	}

	bool Init()
	{
		if (m_window)
		{
			return true;
		}

		m_window = nullptr;
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
			return false;
		}

		m_window = SDL_CreateWindow("Hydra", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			s_screenWidth, s_screenHeight, SDL_WINDOW_SHOWN);

		if (!m_window)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to create SDL window. error={}", SDL_GetError());
			return false;
		}

		m_isOpen = true;

		m_screenSurface = SDL_GetWindowSurface(m_window);

		// Initialize surface.
		SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));

		return true;
	}
	
	void Close()
	{
		if (m_window)
		{
			SDL_DestroyWindow(m_window);
		}

		SDL_Quit();
	}

	bool IsOpen()
	{
		return m_isOpen;
	}

	void Update()
	{
		HandleInput();

		SDL_UpdateWindowSurface(m_window);
	}

private:

	void HandleInput()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			// Handle Quit
			if (e.type == SDL_QUIT)
			{
				m_isOpen = false;
			}
		}
	}

private:

	// 
	bool m_isOpen = false;

	// The window we will render to.
	SDL_Window* m_window = nullptr;

	// The rendering surface covering the entire rendering pane of the window.
	SDL_Surface* m_screenSurface = nullptr;
};

//-------------------------------------------------------------------------------

Game::Game()
	: m_gameController(std::make_unique<GameController>(this))
	, m_client(std::make_unique<GameClient>(this))
	, m_mainWindow(std::make_unique<MainWindow>())
{
	REGISTER_LOGGER("Game");
	s_logger = Log::Logger("Game");
}

Game::~Game()
{
}

bool Game::Init()
{
	m_isInitialized = m_mainWindow->Init();

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
		m_mainWindow->Update();
	}
}

void Game::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

//===============================================================================

} // namespace Game

