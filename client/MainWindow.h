//---------------------------------------------------------------
//
// MainWindow.h
//

#pragma once

#include "client/SDLTypes.h"
#include <glm/vec2.hpp>

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

	// Returns main window width.
	int GetWidth() const { return m_currentSize.x; }

	// Returns main window height.
	int GetHeight() const { return m_currentSize.y; }

	// Returns mouse focus state.
	bool HasMouseFocus() const { return m_hasMouseFocus; }

	// Returns keyboard focus state.
	bool HasKeyboardFocus() const { return m_hasKeyboardFocus; }

	// Returns whether minimized or not.
	bool isMinimized() const { return m_isMinimized; }

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

	// Current window dimensions. width, height.
	glm::ivec2 m_currentSize = {0, 0};

	// Mouse events dispatched from Game.
	bool m_hasMouseFocus = false;

	// Keyboard events dispatched from Game.
	bool m_hasKeyboardFocus = false;

	// True when main window is minimized.
	bool m_isMinimized = false;

	// Whether the window is hidden or not.
	bool m_isHidden = false;
};

//===============================================================================

} // namespace Client
