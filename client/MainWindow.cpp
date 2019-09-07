//---------------------------------------------------------------
//
// MainWindow.cpp
//

#include "MainWindow.h"

#include "common/log.h"
#include "client/RenderEngine.h"

#include <SDL_image.h>
#include <string>
#include <sstream>

namespace Client {

//===============================================================================

namespace {
std::shared_ptr<spdlog::logger> s_logger;
const int32_t s_screenWidth = 2560;
const int32_t s_screenHeight = 1440;
const std::string s_fontFile = "resources/fonts/VL-PGothic-Regular.ttf";
} // anon namespace

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

	// We know that we will need to create a renderer here.
	if (RenderEngine::CreateRenderer(m_window.get()))
	{
		m_renderer = RenderEngine::GetRenderer();
	}

	m_isOpen = true;

	// Initialize the screen surface and renderer.
	m_screenSurface = SDL_GetWindowSurface(m_window.get());

	// Initialize screen with black background.
	SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));

	m_mainFont = TTF_FontPtr(std::move(TTF_OpenFont(s_fontFile.c_str(), 28)), TTF_CloseFont);
	if (!m_mainFont)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to load font. error={}", TTF_GetError());
		return false;
	}

	return true;
}

void MainWindow::Close()
{
	m_isOpen = false;

	m_screenSurface = nullptr;

	m_window.reset();
	m_window = SDL_WindowPtr(nullptr, SDL_DestroyWindow);
}

bool MainWindow::IsOpen() const
{
	return m_isOpen;
}

void MainWindow::Process()
{
	if (!m_renderer)
	{
		return;
	}

	HandleEvents();
	SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(m_renderer);

	//Draw red square
	SDL_Rect fillRect = { 0, 0, 150, 150 };
	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(m_renderer, &fillRect);

	SDL_RenderPresent(m_renderer);
}

void MainWindow::HandleEvents()
{
	m_keyState = SDL_GetKeyboardState(nullptr);

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

void MainWindow::HandleMovement()
{
	// NYI Handle movement from key states..
}

//===============================================================================

} // namespace Client
