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
#include "client/WindowManager.h"

#include <algorithm>
#include <chrono>
#include <SDL.h>

#include <thread>

namespace Client {

//===============================================================================

namespace {
	std::shared_ptr<spdlog::logger> s_logger;
	using frame_period = std::chrono::duration<long long, std::ratio<1, 60>>;
	const float s_desiredFrameRate = 60.0f;
	const float s_msPerSecond = 1000;
	const float s_desiredFrameTime = s_msPerSecond / s_desiredFrameRate;
	const int s_maxUpdateSteps = 6;
	const float s_maxDeltaTime = 1.0f;
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
	auto framePrev = std::chrono::high_resolution_clock::now();
	auto frameCurrent = framePrev;

	auto frameDiff = frameCurrent - framePrev;
	float previousTicks = static_cast<float>(SDL_GetTicks());
	while (m_mainWindow->IsOpen())
	{

		float newTicks = static_cast<float>(SDL_GetTicks());
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;

		// 32 ms in a frame would cause this to be .5, 32ms would be 1.0
		float totalDeltaTime = frameTime / s_desiredFrameTime;

		// Idle loop.
		while (frameDiff < frame_period{ 1 })
		{
			frameCurrent = std::chrono::high_resolution_clock::now();
			frameDiff = frameCurrent - framePrev;

			// We want to render and be handling events no matter what the frame rate is.
			ProcessCallbackQueue();
			ProcessSDLEvents();
			m_renderEngine->Render();
		}

		using hr_duration = std::chrono::high_resolution_clock::duration;
		framePrev = std::chrono::time_point_cast<hr_duration>(framePrev + frame_period{ 1 });
		frameDiff = frameCurrent - framePrev;

		// Time step
		int i = 0;
		while (totalDeltaTime > 0.0f && i < s_maxUpdateSteps)
		{
			float deltaTime = std::min(totalDeltaTime, s_maxDeltaTime);
			m_gameController->Update(deltaTime);
			totalDeltaTime -= deltaTime;
			i++;
		}
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

