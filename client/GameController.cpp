//---------------------------------------------------------------
//
// GameController.cpp
//

#include "GameController.h"

#include "client/scenes/GameScene.h"
#include "client/NetworkController.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace Client {

//===============================================================================

GameController::GameController(sf::RenderWindow* window)
	: m_gameScene(std::make_unique<GameScene>(this, window))
	, m_networkController(new NetworkController)
{
}

GameController::~GameController()
{
}

void GameController::Run()
{
		m_gameScene->ProcessEvents();
		m_gameScene->Update();
		m_gameScene->Draw();

		m_networkController->Process();
}

//===============================================================================

} // namespace Client
