//---------------------------------------------------------------
//
// RenderEngine.h
//

#pragma once

#include "RenderEngineTypes.h"
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
	bool Initialize(SDL_Window* window);

	// Render entry point for every View in the game.
	void Render() const;

	// Static Getters
	static SDL_Renderer* GetRenderer();

private:
	// Ensures that all Views are sorted by z-order
	void UpdateDrawOrder();

	// Initializes the render and binds it to the main window.
	bool CreateRenderer(SDL_Window* window);

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
};

//===============================================================================

} // namespace Client
