//---------------------------------------------------------------
//
// NetworkController.h
//

#pragma once

#include <memory>
#include <vector>
#include <thread>

namespace sf {
	class SocketSelector;
	class TcpSocket;
	class TcpListener;
}

namespace Common {
	class NetworkHelper;
	struct NetworkMessage;
}

namespace Server {

//===============================================================================
class GameServer;
class ServerMessageObserver;
class NetworkController {
public:
	friend class ServerMessageObserver;
	NetworkController(GameServer* gameServer);
	~NetworkController();

	// Spawns a thread that continually reads sockets and checks for new connections.
	void BeginProcessNetworkConnections();
	void Process();

private:

	// Delegate messages we have in the message queue;
	void ProcessMessages();

private:

	// Thread specifically for listening for new connections.
	std::thread m_listenThread;

	// Multiplexer used to read from multiple sockets.
	std::unique_ptr<sf::SocketSelector> m_selector;

	// Will listen for new TCP connections.
	std::unique_ptr<sf::TcpListener> m_socketListener;

	// Socket always ready to be populated with a new client. Moved to list once populated.
	std::unique_ptr<sf::TcpSocket> m_freeSocket;

	// Pointer to game server.
	GameServer* m_gameServer;

	// Helper for sending and receiving messages over the wire.
	std::unique_ptr<Common::NetworkHelper> m_networkHelper;

	// List of currently connected clients.
	std::vector<std::unique_ptr<sf::TcpSocket>> m_clients;

	// Handles events from the message helper.
	std::unique_ptr<ServerMessageObserver> m_messageObserver;

	// Hold the list of messages ready to be delegated.
	std::vector<Common::NetworkMessage> m_pendingMessages;
};

//===============================================================================

} // namespace Server
