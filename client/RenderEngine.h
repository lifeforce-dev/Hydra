//---------------------------------------------------------------
//
// RenderEngine.h
//

#pragma once

#include "RenderEngineTypes.h"

#include <vector>

namespace Client {

//===============================================================================

class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	bool Initialize();

	static SDL_Renderer* GetRenderer();
	static bool CreateRenderer(SDL_Window* window);
};

//===============================================================================

} // namespace Client
