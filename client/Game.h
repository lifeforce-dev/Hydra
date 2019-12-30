//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "common/ThreadSafeQueue.h"
#include "client/SDLTypes.h"

#include <deque>
#include <functional>
#include <memory>

namespace Client {

//===============================================================================

class DebugController;
class DebugEvents;
class Game;
class GameClient;
class GameController;
class InputHandler;
class MainWindow;
class RenderEngine;
class RenderEngineEvents;
class WindowManager;

extern Game* g_game;

class Game
{
public:
	Game();
	~Game();

	// Initialized game systems. Returns true if successful.
	bool Initialize();

	// Run the game.
	void Run();

	// Ensures that any callback passed in here gets executed on the main thread.
	void PostToMainThread(const std::function<void()>& cb);

	// Getters.

	TTF_Font* GetDefaultFont() const { return m_defaultFont.get(); }
	RenderEngine* GetRenderEngine() const { return m_renderEngine.get(); }
	DebugController* GetDebugController() const { return m_debugController.get(); }
	MainWindow* GetMainWindow() const { return m_mainWindow; }

	// Event getters.
	RenderEngineEvents& GetRenderEngineEvents() const { return *m_renderEvents; }
	DebugEvents& GetDebugEvents() const { return *m_debugEvents; }

private:
	void ConnectToServer();
	void ProcessCallbackQueue();

	// Pumps the SDL event queue
	void ProcessSDLEvents();
	void DispatchKeyEvent(SDL_KeyboardEvent* event);
	void DispatchMouseButtonEvent(SDL_MouseButtonEvent* event);
	void DispatchMouseWheelEvent(SDL_MouseWheelEvent* event);
	void DispatchMouseMotionEvent(SDL_MouseMotionEvent* event);
	void DispatchTextInputEvent(SDL_TextInputEvent* event);
	void DispatchWindowEvent(SDL_WindowEvent* event);

private:
	MainWindow* m_mainWindow = nullptr;

	// Functions that need to get processed on the main thread get pushed here.
	Common::ThreadSafeQueue<std::function<void()>> m_callbackQueue;

	// Pull work from the thread safe cb q into here for processing.
	std::deque<std::function<void()>> m_processCbQueue;

	// Don't reorder these.
	std::unique_ptr<GameController> m_gameController;
	std::unique_ptr<GameClient> m_client;

	// Must be initialized before any UI may be initialized.
	std::unique_ptr<RenderEngine> m_renderEngine;
	std::unique_ptr<WindowManager> m_windowManager;
	std::unique_ptr<DebugController> m_debugController;

	SDL_FontPtr m_defaultFont = SDL_FontPtr(nullptr, TTF_CloseFont);

	// Events
	std::unique_ptr<RenderEngineEvents> m_renderEvents;
	std::unique_ptr<DebugEvents> m_debugEvents;

	std::vector<InputHandler*> m_inputHandlers;
};

//===============================================================================

} // namespace Client
