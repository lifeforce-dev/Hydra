//---------------------------------------------------------------
//
// MainWindow.h
//

#pragma once

#include "client/RenderEngineTypes.h"

#include <memory>

namespace Client {

//===============================================================================

class MainWindow {
public:
	MainWindow();
	~MainWindow();

	// Returns true if everything initialized correctly.
	bool Initialize();

	// Close the main window. Systems should listen for this event and shut down.
	void Close();

	// Process window events.
	bool HandleWindowEvent(SDL_WindowEvent* event);

	// Returns the raw SDL window data.
	SDL_Window* GetWindowData() const;

	// Returns whether the window is open or not.
	bool IsOpen() const;

private:
	// Whether we're initialized or not.
	bool m_isOpen = false;

	// The surface covering the entire rendering pane of the window.
	// NOTE: owned by the window and freed as part of destroying the window.
	SDL_Surface* m_screenSurface = nullptr;

	// The main internal window data. Used to provide render space, handle events, etc.
	SDL_WindowPtr m_window = SDL_WindowPtr(nullptr, SDL_DestroyWindow);

	// Current key state. Owned externally.
	const uint8_t* m_keyState = nullptr;
};

//===============================================================================

} // namespace Client
