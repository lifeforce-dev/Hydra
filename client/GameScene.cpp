//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "GameScene.h"

#include "client/Game.h"
#include "client/GameController.h"
#include "client/Label.h"
#include "client/RenderEngine.h"
#include "common/Log.h"
#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"

#include <windows.h>
#include <assert.h>

namespace Client {

//===============================================================================

GameScene::GameScene(GameController* gameController)
	: m_controller(gameController)
	, m_label(nullptr)
{
}

GameScene::~GameScene()
{

}

void GameScene::ProcessEvents()
{
	//TODO:
	// Input handling
	// window poll
}

void GameScene::Update()
{
	// TODO: NYI
}

void GameScene::Render()
{
	// We must have a valid window to draw to.
}

bool GameScene::HandleKeyEvent(SDL_KeyboardEvent* event)
{
	auto sc = event->keysym.scancode;
	if (m_keyboardEventMap.find(sc) == std::cend(m_keyboardEventMap))
	{
		return false;
	}

	m_keyboardEventMap.at(sc)(event);

	return true;
}

bool GameScene::HandleMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	// NYI
	return false;
}

void GameScene::MapKeyboardInput()
{
	m_keyboardEventMap[SDL_Scancode::SDL_SCANCODE_W] = [](SDL_KeyboardEvent* event) {};
	m_keyboardEventMap[SDL_Scancode::SDL_SCANCODE_A] = [](SDL_KeyboardEvent* event) {};
	m_keyboardEventMap[SDL_Scancode::SDL_SCANCODE_S] = [](SDL_KeyboardEvent* event) {};
	m_keyboardEventMap[SDL_Scancode::SDL_SCANCODE_D] = [](SDL_KeyboardEvent* event) {};
}

//===============================================================================

} // namespace Client
