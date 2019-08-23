//---------------------------------------------------------------
//
// GameController.cpp
//

#include "GameController.h"

#include "client/Game.h"
#include "client/scenes/GameScene.h"

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

void GameController::Initialize()
{
	// TODO: init game module and pass main window (render window).
}

void GameController::Run()
{
	if (!m_isInitialized)
	{
		Initialize();
	}

	m_gameScene->ProcessEvents();
	m_gameScene->Update();
	m_gameScene->Draw();

}

//===============================================================================

} // namespace Client
