//---------------------------------------------------------------
//
// RenderEngine.h
//

#pragma once

#include "client/SDLTypes.h"

#include <imgui/imgui.h>
#include <vector>

namespace Client {

//===============================================================================

class View;
class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	// Init render systems (SDL initialization).
	bool Initialize();

	// Render entry point for every View in the game.
	void Render() const;

	SDL_GLContext GetGLContext();

private:
	// Ensures that all Views are sorted by z-order
	void UpdateDrawOrder();

	// Views registered will be rendered every frame.
	void RegisterView(View* view);

	// Once a View is unregistered, it will no longer be rendered.
	void UnregisterView(uint32_t id);

	// Sets up event callbacks for render events.
	void SubscribeEvents();

	// Repaints often necessary in response to certain window events.
	void RepaintWindow();

private:
	std::vector<View*> m_views;
	SDL_GLContext m_glContext = nullptr;
	SDL_Window* m_window = nullptr;
};

//===============================================================================

} // namespace Client
