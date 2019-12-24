//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "common/ThreadSafeQueue.h"
#include "client/RenderEngineTypes.h"

#include <memory>
#include <functional>
#include <deque>

namespace Client {

//===============================================================================

class DebugController;
class DebugEvents;
class Game;
class GameClient;
class GameController;
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
	bool Init();

	// Run the game.
	void Run();

	// Ensures that any callback passed in here gets executed on the main thread.
	void PostToMainThread(const std::function<void()>& cb);

	// Getters.
	TTF_Font* GetDefaultFont() const;
	RenderEngine* GetRenderEngine() const;
	DebugController* GetDebugController() const;

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

	bool m_isInitialized = false;

	// Events
	std::unique_ptr<RenderEngineEvents> m_renderEvents;
	std::unique_ptr<DebugEvents> m_debugEvents;
};

//===============================================================================

} // namespace Client
