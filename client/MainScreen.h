//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "Screen.h"

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
class Application;

class MainScreen : public Screen
{
public:
	MainScreen(Application* application, sf::RenderWindow* window);
	~MainScreen();

	// Screen impl.
	// Main event loop.
	virtual void ProcessEvents() override;

	// Called in the event loop.
	virtual void Draw() override;

private:
	void HandleKeyPress(const sf::Event::KeyEvent& e);
	void ConnectToServer();
	void SendTestMessageToServer();

private:
	sf::RenderWindow* m_window;
	std::unique_ptr<sf::Font> m_font;
	Application* m_application;

	bool m_shouldConnect = false;
};

//===============================================================================

} // namespace Client
