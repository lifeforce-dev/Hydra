//---------------------------------------------------------------
//
// WindowManager.h
//

#pragma once

#include "client/InputHandler.h"

#include <memory>

namespace Client {

//===============================================================================

class MainWindow;
class WindowManager : public InputHandler
{
public:
	WindowManager();

	bool Initialize();

	MainWindow* GetMainWindow() const;

	// InputHandler impl
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) override;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) override;

private:
	std::unique_ptr<MainWindow> m_mainWindow;
};

//===============================================================================

} // namespace Client
