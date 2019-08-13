//---------------------------------------------------------------
//
// NetworkController.h
//

#pragma once

#include <memory>
#include <string>
#include <thread>
#include <observable.hpp>

#include <SFML/System/Clock.hpp>

namespace Common {
	class Connection;
	class NetworkHelper;
	enum struct MessageId : uint32_t;
}

namespace Client {

//===============================================================================

class ClientTcpSocket;
class Game;
class NetworkController
{
public:
	NetworkController(Game* game);
	~NetworkController();

	// Connect to the server at the specified address.
	void ConnectToServer(const std::string& address = std::string(), int port = 0);

	// Will queue a message up to get sent to the server.
	void SendMessageToServer(Common::MessageId type, const std::string& message);

	// Called from the main loop. This will send any queued messages and receive anything from over the wire.
	void Process();

	// Returns whether we're currently connected to the server.
	bool IsConnected() const;

private:
	void OnConnectedToServer();
	void UpdateConnectionStatus();

	std::unique_ptr<Common::NetworkHelper> m_networkHelper;
	std::unique_ptr<ClientTcpSocket> m_socket;
	std::unique_ptr<sf::Clock> m_retryTimer;

	Game* m_game;

	// True if we're connected to the game server.
	bool m_isConnected = false;
};

//===============================================================================

} // namespace Client
