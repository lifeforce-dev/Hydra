//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "Scene.h"

#include <SFML/Window/Event.hpp>
#include <memory>

namespace sf {
	class RenderWindow;
	class Font;
}

namespace Common {
	class Connection;
	class NetworkHelper;
}

namespace Client {

//===============================================================================

class NetworkController;
class GameController;

class GameScene : public Scene
{
public:
	GameScene(GameController* gameController, sf::RenderWindow* window);
	~GameScene();

	// Screen impl.

	// Will pump all event queues here.
	virtual void ProcessEvents() override;

	// Updates resulting from the event processing will be handled here.
	virtual void Update() override;


	// Called in the event loop.
	virtual void Draw() override;

	//---------------------------------------------------------------------------

private:
	void HandleKeyPress(const sf::Event::KeyEvent& e);
	void SendTestMessageToServer();

private:
	sf::RenderWindow* m_window;
	std::unique_ptr<sf::Font> m_font;
	GameController* m_application;
};

//===============================================================================

} // namespace Client
