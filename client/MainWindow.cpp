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
#include <GL/glew.h>
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
}

bool MainWindow::Initialize()
{
	if (m_window)
	{
		SPDLOG_LOGGER_WARN(s_logger, "Subsequent attempts to init main window. Ignoring...");
		return true;
	}

	const std::string glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_WindowFlags windowFlags = static_cast<SDL_WindowFlags>(
		(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));

	// Init window.
	m_window = SDL_WindowPtr(std::move(SDL_CreateWindow("Hydra",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			s_defaultWidth,
			s_defaultHeight,
			windowFlags)), SDL_DestroyWindow);

	if (!m_window)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create SDL window. error={}", SDL_GetError());
		return false;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(m_window.get());
	SDL_GL_MakeCurrent(m_window.get(), glContext);
	SDL_GL_SetSwapInterval(0);

	// Initialize glew. Since we're creating the window and the context anyway.
	if (glewInit() != GLEW_OK)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize glew. error={}", SDL_GetError());
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
