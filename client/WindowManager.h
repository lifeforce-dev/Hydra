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
	~WindowManager();

	bool Initialize();

	MainWindow* GetMainWindow() const;
	bool HandleWindowEvent(SDL_WindowEvent* event);
	void ShutDown();

	// InputHandler impl
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) override;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) override;
	virtual bool HandleMouseWheelEvent(SDL_MouseWheelEvent* event) override;
	virtual bool HandleMouseMotionEvent(SDL_MouseMotionEvent* event) override;
	virtual bool HandleTextInputEvent(SDL_TextInputEvent* event) override;

private:
	std::unique_ptr<MainWindow> m_mainWindow;
};

//===============================================================================

} // namespace Client
