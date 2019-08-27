//---------------------------------------------------------------
//
// MainWindow.h
//

#pragma once

#include <memory>
#include <SDL.h>

namespace Client {

//===============================================================================

using SDL_SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;
using SDL_TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using SDL_RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using SDL_WindowPtr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;

class MainWindow {
public:
	MainWindow();
	~MainWindow();

	bool Init();
	void Close();
	bool IsOpen() const;
	void Process();

private:
	void HandleEvents();
	void HandleKeyEvent(const SDL_Event& e);
	void HandleMouseEvent(const SDL_Event& e);
	void HandleWindowEvent(const SDL_Event& e);

private:

	// Whether the window is open or not.
	bool m_isOpen = false;

	// The surface covering the entire rendering pane of the window.
	// NOTE: owned by the window and freed as part of destroying the window.
	SDL_Surface* m_screenSurface = nullptr;

	// The main internal window data. Used to provide render space, handle events, etc.
	SDL_WindowPtr m_window = SDL_WindowPtr(nullptr, SDL_DestroyWindow);

	// Render state and API for rendering.
	SDL_RendererPtr m_renderer = SDL_RendererPtr(nullptr, SDL_DestroyRenderer);

	// Current key state. Owned externally.
	const uint8_t* m_keyState = nullptr;
};

//===============================================================================

} // namespace Client
