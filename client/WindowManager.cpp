//---------------------------------------------------------------
//
// WindowManager.cpp
//

#include "WindowManager.h"

#include "client/MainWindow.h"

#include "common/log.h"

namespace Client {

//===============================================================================

namespace {
	std::shared_ptr<spdlog::logger> s_logger;
}

WindowManager::WindowManager() 
	: m_mainWindow(std::make_unique<MainWindow>())
{
	REGISTER_LOGGER("WindowManager");
	s_logger = Log::Logger("WindowManager");
}

bool WindowManager::Initialize()
{
	if (!m_mainWindow->Initialize())
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize main window.");
		return false;
	}

	return true;
}

MainWindow* WindowManager::GetMainWindow() const
{
	return m_mainWindow.get();
}

bool WindowManager::HandleWindowEvent(SDL_WindowEvent* event)
{
	m_mainWindow->HandleWindowEvent(event);
	return false;
}

void WindowManager::ShutDown()
{
	m_mainWindow->Close();
}

bool WindowManager::HandleKeyEvent(SDL_KeyboardEvent* event)
{
	// NYI
	return false;
}

bool WindowManager::HandleMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	// NYI
	return false;
}

//===============================================================================

} // namespace Client
