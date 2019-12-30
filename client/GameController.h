//---------------------------------------------------------------
//
// GameController.h
//

#pragma once

#include "InputHandler.h"

#include <memory>

namespace Client {

//===============================================================================

class Game;
class GameScene;
class NetworkController;

class GameController : public InputHandler
{
public:
	GameController(Game* game);
	~GameController();

	void Initialize();
	void Update();

	// InputHandler impl
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) override;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) override;
	virtual bool HandleMouseWheelEvent(SDL_MouseWheelEvent* event) override;
	virtual bool HandleMouseMotionEvent(SDL_MouseMotionEvent* event) override;
	virtual bool HandleTextInputEvent(SDL_TextInputEvent* event) override;

private:

	Game* m_game;
	std::unique_ptr<GameScene> m_gameScene;

	bool m_isInitialized = false;
};

//===============================================================================

} // namespace Client
