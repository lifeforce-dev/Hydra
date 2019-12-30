//---------------------------------------------------------------
//
// GameController.cpp
//

#include "GameController.h"

#include "client/Game.h"
#include "client/GameScene.h"

namespace Client {

//===============================================================================

GameController::GameController(Game* game)
	: m_game(game)
	, m_gameScene(std::make_unique<GameScene>(this))
{
}

GameController::~GameController()
{
}


void GameController::Update()
{
	m_gameScene->Update();
}

bool GameController::HandleKeyEvent(SDL_KeyboardEvent* event)
{
	return m_gameScene->HandleKeyEvent(event);
}

bool GameController::HandleMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	return m_gameScene->HandleMouseButtonEvent(event);
}

bool GameController::HandleMouseWheelEvent(SDL_MouseWheelEvent* event)
{
	// NYI
	return false;
}

bool GameController::HandleMouseMotionEvent(SDL_MouseMotionEvent* event)
{
	// NYI
	return false;
}

bool GameController::HandleTextInputEvent(SDL_TextInputEvent* event)
{
	// NYI
	return false;
}

void GameController::Initialize()
{
	m_gameScene->Initialize();
}

//===============================================================================

} // namespace Client
