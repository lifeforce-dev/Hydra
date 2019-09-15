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

//===============================================================================

} // namespace Client
