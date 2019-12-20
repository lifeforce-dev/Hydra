//---------------------------------------------------------------
//
// RenderEngine.h
//

#pragma once

#include "RenderEngineTypes.h"
#include <vector>

namespace Client {

//===============================================================================

class Renderable;
class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	// Init render systems (SDL initialization).
	bool Initialize(SDL_Window* window);

	// Renderables registered will be rendered every frame.
	void RegisterRenderable(Renderable* renderable);

	// Once a renderable is unregistered, it will no longer be rendered.
	void UnregisterRenderable(uint32_t id);

	// Render entry point for every renderable in the game.
	void Render() const;

	// Static Getters
	static SDL_Renderer* GetRenderer();

private:
	// Ensures that all renderables are sorted by z-order
	void UpdateDrawOrder();

	// Initializes the render and binds it to the main window.
	bool CreateRenderer(SDL_Window* window);

private:
	std::vector<Renderable*> m_renderables;
};

//===============================================================================

} // namespace Client
