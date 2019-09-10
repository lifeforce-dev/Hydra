//---------------------------------------------------------------
//
// MainWindow.cpp
//

#include "MainWindow.h"

#include "client/Game.h"
#include "client/RenderEngine.h"
#include "common/log.h"

#include <string>
#include <sstream>

namespace Client {

//===============================================================================

namespace {

// Logger for main window.
std::shared_ptr<spdlog::logger> s_logger;

// Window dimensions.
const int32_t s_screenWidth = 2560;
const int32_t s_screenHeight = 1440;
} // anon namespace

//-------------------------------------------------------------------------------

MainWindow::MainWindow()
{
	REGISTER_LOGGER("MainWindow");
	s_logger = Log::Logger("MainWindow");
}

MainWindow::~MainWindow()
{
	Close();
}

bool MainWindow::Initialize()
{
	if (m_window)
	{
		SPDLOG_LOGGER_WARN(s_logger, "Subsequent attempts to init main window. Ignoring...");
		return true;
	}

	// Init window.
	m_window = SDL_WindowPtr(std::move(SDL_CreateWindow("Hydra",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			s_screenWidth,
			s_screenHeight,
			SDL_WINDOW_SHOWN)), SDL_DestroyWindow);

	if (!m_window)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create SDL window. error={}", SDL_GetError());
		return false;
	}

	m_isOpen = true;

	// Initialize the screen surface and renderer.
	m_screenSurface = SDL_GetWindowSurface(m_window.get());

	// Initialize screen with black background.
	SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));

	return true;
}

void MainWindow::Render()
{
	//Draw red square
	SDL_Rect fillRect = { 0, 0, 150, 150 };
	SDL_SetRenderDrawColor(RenderEngine::GetRenderer(), 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(RenderEngine::GetRenderer(), &fillRect);
}

void MainWindow::Close()
{
	m_isOpen = false;
	m_screenSurface = nullptr;

	m_window.reset();
}

bool MainWindow::IsOpen() const
{
	return m_isOpen;
}

void MainWindow::Process()
{
	HandleEvents();
}

SDL_Window* MainWindow::GetWindowData() const
{
	return m_window.get();
}

void MainWindow::HandleEvents()
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
			HandleKeyEvent(e);
			break;
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			HandleMouseEvent(e);
			break;
		case SDL_WINDOWEVENT:
			HandleWindowEvent(e);
			break;
		case SDL_QUIT:
			m_isOpen = false;
			break;
		default:
			break;
		}
	}
}

void MainWindow::HandleKeyEvent(const SDL_Event& e)
{
	// NYI
}

void MainWindow::HandleMouseEvent(const SDL_Event& e)
{
	// NYI
}

void MainWindow::HandleWindowEvent(const SDL_Event& e)
{
	switch (e.window.event)
	{
	case SDL_WINDOWEVENT_ENTER:
		break;
	case SDL_WINDOWEVENT_LEAVE:
		break;
	default:
		break;
	}
}

//===============================================================================

} // namespace Client
