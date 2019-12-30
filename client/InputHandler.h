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
public:
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) = 0;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) = 0;
	virtual bool HandleMouseWheelEvent(SDL_MouseWheelEvent* event) = 0;
	virtual bool HandleMouseMotionEvent(SDL_MouseMotionEvent* event) = 0;
	virtual bool HandleTextInputEvent(SDL_TextInputEvent* event) = 0;
};

//===============================================================================

} // namespace Client
