//---------------------------------------------------------------
//
// MainWindow.h
//

#pragma once

#include "client/RenderEngineTypes.h"


#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

namespace Client {

//===============================================================================

class MainWindow {
public:
	MainWindow();
	~MainWindow();

	bool Initialize();
	void Close();
	bool IsOpen() const;
	void Process();

private:
	void HandleEvents();
	void HandleKeyEvent(const SDL_Event& e);
	void HandleMouseEvent(const SDL_Event& e);
	void HandleWindowEvent(const SDL_Event& e);
	void HandleMovement();

private:

	// Whether we're initialized or not.
	bool m_isOpen = false;

	// The surface covering the entire rendering pane of the window.
	// NOTE: owned by the window and freed as part of destroying the window.
	SDL_Surface* m_screenSurface = nullptr;

	// Render state and API for rendering.
	SDL_Renderer* m_renderer = nullptr;

	// The main internal window data. Used to provide render space, handle events, etc.
	SDL_WindowPtr m_window = SDL_WindowPtr(nullptr, SDL_DestroyWindow);

	// Main game font.
	TTF_FontPtr m_mainFont = TTF_FontPtr(nullptr, TTF_CloseFont);

	// Current key state. Owned externally.
	const uint8_t* m_keyState = nullptr;
};

//===============================================================================

} // namespace Client
