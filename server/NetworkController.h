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
}

namespace Server {

//===============================================================================
class GameServer;
class NetworkController {
public:
	NetworkController(GameServer* gameServer);
	~NetworkController();

	void BeginAcceptingConnections();
	void Process();

private:
	// helpers

private:

	// Thread specifically for listening for new connections.
	std::thread m_listenThread;
	std::unique_ptr<sf::SocketSelector> m_selector;
	std::unique_ptr<Common::NetworkHelper> m_networkHelper;
	std::unique_ptr<sf::TcpListener> m_socketListener;

	std::vector<std::unique_ptr<sf::TcpSocket>> m_sockets;
	std::unique_ptr<sf::TcpSocket> m_freeSocket;

	GameServer* m_gameServer;

	class ThreadSafeSocketHelper
	{
		void FillFreeSocket();

	private:
	};
};

//===============================================================================

} // namespace Server
