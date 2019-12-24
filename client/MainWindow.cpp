//---------------------------------------------------------------
//
// MainWindow.cpp
//

#include "MainWindow.h"

#include "client/Game.h"
#include "client/RenderEngine.h"
#include "client/RenderEngineEvents.h"
#include "common/log.h"

#include <assert.h>
#include <string>
#include <sstream>

namespace Client {

//===============================================================================

namespace {

// Logger for main window.
std::shared_ptr<spdlog::logger> s_logger;

// Window dimensions.
const int32_t s_defaultWidth = 1280;
const int32_t s_defaultHeight = 720;
} // anon namespace

//-------------------------------------------------------------------------------

MainWindow::MainWindow()
{
	REGISTER_LOGGER("MainWindow");
	s_logger = Log::Logger("MainWindow");
}

MainWindow::~MainWindow()
{
	// Ensure that we closed properly before shutting down.
	assert(!m_window);
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
			s_defaultWidth,
			s_defaultHeight,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)), SDL_DestroyWindow);

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

	m_hasKeyboardFocus = true;
	m_hasMouseFocus = true;
	m_currentSize = { s_defaultWidth, s_defaultHeight };

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
	auto& renderEvents = g_game->GetRenderEngineEvents();
	switch (event->event)
	{
	case SDL_WINDOWEVENT_ENTER:
	{
		m_hasMouseFocus = true;
		return true;
	}
	case SDL_WINDOWEVENT_LEAVE:
	{
		m_hasMouseFocus = false;
		return true;
	}
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	{
		m_currentSize.x = event->data1;
		m_currentSize.y = event->data2;
		renderEvents.GetMainWindowSizeChangedEvent().notify();
		return true;
	}
	case SDL_WINDOWEVENT_EXPOSED:
	{
		renderEvents.GetMainWindowExposedEvent().notify();
		return true;
	}
	case SDL_WINDOWEVENT_FOCUS_GAINED:
	{
		m_hasKeyboardFocus = true;
		return true;
	}
	case SDL_WINDOWEVENT_FOCUS_LOST:
	{
		m_hasKeyboardFocus = false;
		return true;
	}
	case SDL_WINDOWEVENT_MINIMIZED:
	{
		m_isMinimized = true;
		return true;
	}
	case SDL_WINDOWEVENT_MAXIMIZED:
	{
		m_isMinimized = false;
		return true;
	}
	case SDL_WINDOWEVENT_RESTORED:
	{
		m_isMinimized = false;
		return true;
	}
	case SDL_WINDOWEVENT_RESIZED:
	{
		m_currentSize.x = event->data1;
		m_currentSize.y = event->data2;
		renderEvents.GetMainWindowSizeChangedEvent().notify();
		return true;
	}
	case SDL_WINDOWEVENT_SHOWN:
	{
		m_isHidden = false;
		return true;
	}
	case SDL_WINDOWEVENT_HIDDEN:
	{
		m_isHidden = true;
		return true;
	}
	case SDL_WINDOWEVENT_MOVED:
	{
		// Nothing to do here.
		return true;
	}
	default:
		SPDLOG_LOGGER_ERROR(s_logger, "SDL_WindowEvent unhandled. event={}", event->event);
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
