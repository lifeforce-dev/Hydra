//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "common/Timer.h"
#include "client/GameClient.h"
#include "client/GameController.h"
#include "client/MainWindow.h"
#include "client/RenderEngine.h"
#include "client/WindowManager.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <spdlog/fmt/bundled/chrono.h>
#include <SDL.h>

namespace Client {

//===============================================================================

namespace {
	std::shared_ptr<spdlog::logger> s_logger;
}

Game* g_game = nullptr;

//-------------------------------------------------------------------------------

Game::Game()
	: m_renderEvents(std::make_unique<RenderEngineEvents>())
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
	// Init SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
		return false;
	}

	m_windowManager = std::make_unique<WindowManager>();
	
	if (m_windowManager->Initialize())
	{
		m_mainWindow = m_windowManager->GetMainWindow();
		m_renderEngine = std::make_unique<RenderEngine>();
	}

	m_isInitialized = m_renderEngine->Initialize(m_mainWindow->GetWindowData());

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
	m_gameController->Initialize();
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

void Game::ProcessSDLEvents()
{
	// Pump the event queue...
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		// Handle single key events here
		switch (e.type)
		{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			DispatchKeyEvent(&e.key);
			break;
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			DispatchMouseButtonEvent(&e.button);
			break;
		case SDL_WINDOWEVENT:
			DispatchWindowEvent(&e.window);
			break;
		case SDL_QUIT:
			// Handle shut down.
			SDL_Quit();
			m_windowManager->ShutDown();
			break;
		default:
			break;
		}
	}
}

void Game::DispatchKeyEvent(SDL_KeyboardEvent* event)
{
	// Always allow the UI to handle first. If unhandled, then pass to GameController.
	bool handled = m_windowManager->HandleKeyEvent(event);
	if (!handled)
	{
		m_gameController->HandleKeyEvent(event);
	}
}

void Game::DispatchMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	// Always allow the UI to handle first. If unhandled, then pass to GameController.
	bool handled = m_windowManager->HandleMouseButtonEvent(event);
	if (!handled)
	{
		m_gameController->HandleMouseButtonEvent(event);
	}
}

void Game::DispatchWindowEvent(SDL_WindowEvent* event)
{
	m_windowManager->HandleWindowEvent(event);
}

void Game::Run()
{
	ConnectToServer();

	using namespace std::chrono;
	using namespace std::chrono_literals;
	using Clock = steady_clock;
	auto constexpr dt = duration<long long, std::ratio<1, 60>>{ 1 };

	using duration = decltype(Clock::duration{} + dt);
	using time_point = time_point<Clock, duration>;

	// Time step accumulator
	duration accumulator = 0s;

	// Actual time between frames
	duration frameDt = 0us;

	// Used for keeping track of how long it takes to complete a frame.
	time_point previousTime = Clock::now();
	time_point currentTime = Clock::now();
	while (m_mainWindow->IsOpen())
	{
		time_point newTime = Clock::now();
		frameDt = newTime - previousTime;
		if (frameDt > dt)
		{
			// 00.000 seconds
			SPDLOG_LOGGER_WARN(s_logger, "Call to Update runtime exceeded 16ms. time={:%S}",
				frameDt);
		}

		previousTime = newTime;

		auto frameTime = newTime - currentTime;
		if (frameTime > 250ms)
		{
			frameTime = 250ms;
		}

		currentTime = newTime;
		accumulator += frameTime;

		// Runs exactly every 16ms. Unless it doesn't, then its logged.
		while (accumulator >= dt)
		{
			m_gameController->Update();

			// Not actually doing anything with the time step for now.
			accumulator -= dt;
		}
		// Render and be handling events independent of the update loop.
		ProcessCallbackQueue();
		ProcessSDLEvents();
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

