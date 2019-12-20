//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "client/InputHandler.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace Common {
	class Connection;
	class NetworkHelper;
}

namespace Client {

//===============================================================================

class Label;
class NetworkController;
class GameController;

class GameScene : public InputHandler
{
public:
	GameScene(GameController* gameController);
	~GameScene();

	// Scene impl.

	// Will pump all event queues here.
	void ProcessEvents();

	// Updates resulting from the event processing will be handled here.
	void Update();

	// Called in the event loop.
	void Render();

	// InputHandler impl
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) override;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) override;

private:
	void MapKeyboardInput();

private:
	GameController* m_controller;
	std::unique_ptr<Label> m_label;
	std::unordered_map<SDL_Scancode, std::function<void(SDL_KeyboardEvent* )>> m_keyboardEventMap;
};

//===============================================================================

} // namespace Client
