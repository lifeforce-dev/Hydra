//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "GameScene.h"

#include "client/GameController.h"
#include "client/NetworkController.h"
#include "common/Log.h"
#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include <windows.h>
#include <assert.h>

namespace Client {

//===============================================================================

namespace
{
	const std::string s_fontPath = "resources/fonts/arial.ttf";
}

GameScene::GameScene(GameController* gameController, sf::RenderWindow* window)
	: m_window(window)
	, m_font(new sf::Font)
	, m_application(gameController)
{
	if (!m_font->loadFromFile(s_fontPath))
	{
		LOG_DEBUG("Error: Could not find font. path=" + s_fontPath);
	}
}

GameScene::~GameScene()
{

}

void GameScene::ProcessEvents()
{
	sf::Event e;
	while (m_window->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			m_window->close();
		}

		if (e.type == sf::Event::KeyPressed)
		{
			HandleKeyPress(e.key);
		}
	}
}

void GameScene::Update()
{
	// TODO: NYI
}

void GameScene::Draw()
{
	// We must have a valid window to draw to.
	assert(m_window);
	m_window->clear(sf::Color::Black);

	m_window->display();
}

void GameScene::HandleKeyPress(const sf::Event::KeyEvent& e)
{
	switch (e.code)
	{
	case sf::Keyboard::S:
		SendTestMessageToServer();
		break;
	default:
		break;
	}
}

void GameScene::SendTestMessageToServer()
{
}

//===============================================================================

} // namespace Client
