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
const int32_t s_screenWidth = 1280;
const int32_t s_screenHeight = 720;
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

void MainWindow::Close()
{
	m_isOpen = false;
	m_screenSurface = nullptr;

	m_window.reset();
}

bool MainWindow::HandleWindowEvent(SDL_WindowEvent* event)
{
	switch (event->type)
	{
	case SDL_WINDOWEVENT_ENTER:
		return true;
	case SDL_WINDOWEVENT_LEAVE:
		return true;
	default:
		SPDLOG_LOGGER_ERROR(s_logger, "Unkown SDL_WindowEvent unhandled. event={}", event->windowID);
		return false;
	}

	return false;
}

bool MainWindow::IsOpen() const
{
	return m_isOpen;
}

SDL_Window* MainWindow::GetWindowData() const
{
	return m_window.get();
}

//===============================================================================

} // namespace Client
