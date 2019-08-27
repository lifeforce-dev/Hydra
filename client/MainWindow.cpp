//---------------------------------------------------------------
//
// MainWindow.cpp
//

#include "MainWindow.h"

#include "common/log.h"

#include <SDL_image.h>
#include <string>
#include <sstream>

namespace Client {

//===============================================================================

namespace {
std::shared_ptr<spdlog::logger> s_logger;
const int32_t s_screenWidth = 2560;
const int32_t s_screenHeight = 1440;
}

MainWindow::MainWindow()
{
	REGISTER_LOGGER("MainWindow");
	s_logger = Log::Logger("MainWindow");
}

MainWindow::~MainWindow()
{
	Close();
}

bool MainWindow::Init()
{
	if (m_window)
	{
		SPDLOG_LOGGER_WARN(s_logger, "Subsequent attempts to init main window. Ignoring...");
		return true;
	}

	// Init SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
		return false;
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

	m_renderer = SDL_RendererPtr(std::move(SDL_CreateRenderer(
		m_window.get(), -1, SDL_RENDERER_ACCELERATED)), SDL_DestroyRenderer);

	if (!m_renderer)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create renderer", SDL_GetError());
	}

	SDL_SetRenderDrawColor(m_renderer.get(), 0x00, 0x00, 0x00, 0xff);

	// Init PNG loader.
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to load png. error= {}", IMG_GetError());
		return false;
	}

	return true;
}

void MainWindow::Close()
{
	m_isOpen = false;

	m_renderer.reset();
	m_renderer = SDL_RendererPtr(nullptr, SDL_DestroyRenderer);

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
	HandleEvents();
	SDL_RenderClear(m_renderer.get());
	
	// NYI Render code goes here.

	SDL_RenderPresent(m_renderer.get());
}

void MainWindow::HandleEvents()
{
	// Pump the event queue...
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		HandleKeyEvent(e);
		HandleMouseEvent(e);
		HandleWindowEvent(e);
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
	// NYI
}

//===============================================================================

} // namespace Client
