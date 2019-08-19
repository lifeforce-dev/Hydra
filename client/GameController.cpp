//---------------------------------------------------------------
//
// GameController.cpp
//

#include "GameController.h"

#include "client/Game.h"
#include "client/scenes/GameScene.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace Client {

//===============================================================================

GameController::GameController(Game* game)
	: m_game(game)
{
}

GameController::~GameController()
{
}

void GameController::Initialize()
{
	m_gameScene = std::make_unique<GameScene>(this, m_game->GetMainWindow());
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
