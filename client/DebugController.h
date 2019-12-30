//---------------------------------------------------------------
//
// DebugController.h
//

#pragma once

#include "client/InputHandler.h"

#include <imgui.h>
#include <SDL.h>

namespace Client {

//===============================================================================

struct PerformanceData
{
	// How long it took to render this frame.
	int renderDurationSeconds = 0;

	// How long it took to process simulation updates this frame.
	int simDurationSeconds = 0;

	// How long it took to process all events for this frame.
	int eventHandleDurationSeconds = 0;

	// How long it took to process an entire frame.
	int fullFrameTime = 0;
};

class DebugController : public InputHandler
{
public:
	DebugController();
	~DebugController();
	void SubscribeEvents();
	void Initialize(SDL_Window* window);
	void Render();

	// InputHandler impl
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) override;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) override;
	virtual bool HandleMouseWheelEvent(SDL_MouseWheelEvent* event) override;
	virtual bool HandleMouseMotionEvent(SDL_MouseMotionEvent* event) override;
	virtual bool HandleTextInputEvent(SDL_TextInputEvent* event) override;

private:
	PerformanceData m_windowData;
	SDL_Window* m_window;
};

//===============================================================================

} // namespace Client
