//---------------------------------------------------------------
//
// InputHandler.h
//

#pragma once

#include <SDL.h>

namespace Client {

//===============================================================================

class InputHandler
{
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) = 0;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) = 0;
};

//===============================================================================

} // namespace Client
