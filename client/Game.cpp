//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "common/Timer.h"
#include "client/DebugController.h"
#include "client/DebugEvents.h"
#include "client/GameClient.h"
#include "client/GameController.h"
#include "client/MainWindow.h"
#include "client/RenderEngine.h"
#include "client/RenderEngineEvents.h"
#include "client/WindowManager.h"

#include <algorithm>
#include <chrono>
#include <imgui.h>
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
	, m_debugController(std::make_unique<DebugController>())
	, m_debugEvents(std::make_unique<DebugEvents>())
	, m_renderEngine(std::make_unique<RenderEngine>())
	, m_windowManager(std::make_unique<WindowManager>())
{
	REGISTER_LOGGER("Game");
	s_logger = Log::Logger("Game");
	g_game = this;
}


Game::~Game()
{
	g_game = nullptr;
}

bool Game::Initialize()
{
	// Init SDL.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
		return false;
	}

	m_renderEngine->Initialize();
	
	if (m_windowManager->Initialize())
	{
		m_mainWindow = m_windowManager->GetMainWindow();
	}
	m_debugController->Initialize(m_mainWindow->GetWindowData());

	// TODO: Pull this from a file with a FontManager.
	m_defaultFont = SDL_FontPtr(std::move(
		TTF_OpenFont("resources/fonts/Charter Regular.ttf", 24)), TTF_CloseFont);

	if (!m_defaultFont)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to load font. error=", TTF_GetError());
		return false;
	}

	m_gameController = std::make_unique<GameController>(this);
	m_gameController->Initialize();
	m_client = std::make_unique<GameClient>(this);

	// Debug UI should always get first dibs on input events.
	m_inputHandlers.push_back(m_debugController.get());

	// If DebugController doesn't handle the event, it goes to game UI.
	m_inputHandlers.push_back(m_windowManager.get());

	// After game UI it goes to the GameController. Player input events would go her, for example.
	m_inputHandlers.push_back(m_gameController.get());

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
			DispatchMouseMotionEvent(&e.motion);
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			DispatchMouseButtonEvent(&e.button);
			break;
		case SDL_MOUSEWHEEL:
			DispatchMouseWheelEvent(&e.wheel);
			break;
		case SDL_WINDOWEVENT:
			DispatchWindowEvent(&e.window);
			break;
		case SDL_TEXTINPUT:
			DispatchTextInputEvent(&e.text);
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
	// Events handled in the order m_inputHandlers is initialized in.
	for (auto handler : m_inputHandlers)
	{
		if (handler->HandleKeyEvent(event))
			break;
	}
}

void Game::DispatchMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	// Events handled in the order m_inputHandlers is initialized in.
	for (auto handler : m_inputHandlers)
	{
		if (handler->HandleMouseButtonEvent(event))
			break;
	}
}

void Game::DispatchMouseWheelEvent(SDL_MouseWheelEvent* event)
{
	// Events handled in the order m_inputHandlers is initialized in.
	for (auto handler : m_inputHandlers)
	{
		if (handler->HandleMouseWheelEvent(event))
			break;
	}
}

void Game::DispatchMouseMotionEvent(SDL_MouseMotionEvent* event)
{
	// Events handled in the order m_inputHandlers is initialized in.
	for (auto handler : m_inputHandlers)
	{
		if (handler->HandleMouseMotionEvent(event))
			break;
	}
}

void Game::DispatchTextInputEvent(SDL_TextInputEvent* event)
{
	// Events handled in the order m_inputHandlers is initialized in.
	for (auto handler : m_inputHandlers)
	{
		if (handler->HandleTextInputEvent(event))
			break;
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
	duration frameDt = 0ms;

	// Used for keeping track of how long it takes to complete a frame.
	time_point previousTime = Clock::now();
	time_point currentTime = Clock::now();
	while (m_mainWindow->IsOpen())
	{
		time_point newTime = Clock::now();
		frameDt = newTime - previousTime;

		m_debugEvents->GetFullFrameCompletedEvent().notify(duration_cast<microseconds>(frameDt));
		if (frameDt > dt)
		{
			// 00.000 seconds
			SPDLOG_LOGGER_WARN(s_logger, "Single frame runtime exceeded 16ms. time={:%S}",
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

//===============================================================================

} // namespace Game
