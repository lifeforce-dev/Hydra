//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "GameScene.h"

#include "client/Game.h"
#include "client/GameController.h"
#include "client/GameObjectView.h"
#include "client/Label.h"
#include "client/RenderEngineEvents.h"

#include "common/Log.h"
#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"

#include <glm/vec2.hpp>
#include <windows.h>
#include <assert.h>

namespace Client {

//===============================================================================

GameScene::GameScene(GameController* gameController)
	: m_controller(gameController)
	, m_gameObjectView(std::make_unique<GameObjectView>())
{
}

GameScene::~GameScene()
{

}

void GameScene::Initialize()
{
	m_viewData.position = { 200.0f, 200.0f };
	m_viewData.size = { 100.0f, 100.0f };
	m_data.speed = 5.0f;
	m_gameObjectView->Initialize(&m_viewData);
	g_game->GetRenderEngineEvents().GetViewCreatedEvent().notify(m_gameObjectView.get());
}

void GameScene::Update(float deltaTime)
{
	CalculateDirection();
	auto magnitude = (m_data.speed * m_data.currentDirection) * deltaTime;
	m_viewData.position += magnitude;
}

bool GameScene::HandleKeyEvent(SDL_KeyboardEvent* event)
{
	if (m_keyboardEventMap.empty())
	{
		return false;
	}

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
	// NYI
}

void GameScene::CalculateDirection()
{
	glm::vec2 newDirection = { 0.0f, 0.0f };

	const Uint8* state = SDL_GetKeyboardState(nullptr);
	if (state[SDL_SCANCODE_W])
	{
		newDirection += glm::vec2{ 0.0f, -1.0f };
	}

	if (state[SDL_SCANCODE_A])
	{
		newDirection += glm::vec2{ -1.0f, 0.0f };
	}

	if (state[SDL_SCANCODE_S])
	{
		newDirection += glm::vec2{ 0.0f, 1.0f };
	}

	if (state[SDL_SCANCODE_D])
	{
		newDirection += glm::vec2{ 1.0f, 0.0f };
	}
	
	m_data.currentDirection = newDirection;
}

//===============================================================================

} // namespace Client
