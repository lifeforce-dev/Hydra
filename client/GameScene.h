//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "client/GameObjectData.h"
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

class GameObject;
class Label;
class NetworkController;
class GameController;
class GameObjectView;

class GameScene : public InputHandler
{
public:
	GameScene(GameController* gameController);
	~GameScene();

	// Creates player, sets up events, and other setup tasks.
	void Initialize();

	// Updates resulting from the event processing will be handled here.
	void Update(float deltaTime);

	// InputHandler impl
	virtual bool HandleKeyEvent(SDL_KeyboardEvent* event) override;
	virtual bool HandleMouseButtonEvent(SDL_MouseButtonEvent* event) override;

private:
	void MapKeyboardInput();
	void CalculateDirection();

private:
	GameObjectViewData m_viewData = {};
	GameObjectData m_data = {};
	std::unique_ptr<GameObjectView> m_gameObjectView;
	GameController* m_controller;
	std::unordered_map<SDL_Scancode, std::function<void(SDL_KeyboardEvent* )>> m_keyboardEventMap;
};

//===============================================================================

} // namespace Client
