//---------------------------------------------------------------
//
// GameController.cpp
//

#include "GameController.h"

#include "Game.h"
#include "NetworkController.h"
#include "scenes/GameScene.h"

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
	m_networkController = m_game->GetNetworkController();
	m_gameScene = std::make_unique<GameScene>(this, m_game->GetMainWindow());
}

void GameController::Run()
{
	if (!m_isInitialized)
	{
		Initialize();
	}

	m_networkController->Process();
	m_gameScene->ProcessEvents();
	m_gameScene->Update();
	m_gameScene->Draw();

}

//===============================================================================

} // namespace Client
