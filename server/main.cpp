//---------------------------------------------------------------
//
// main.cpp
//

#include "common/Log.h"
#include "common/NetworkHelper.h"

#include <memory>
#include <SFML/Network.hpp>

namespace {
	const uint32_t s_port = 50001;
	const uint32_t s_minConnectedClients = 1;
}


//-----------------------------------------------------------------------------------

class Server
{
public:
	Server()
		: m_socket(std::make_unique<sf::TcpSocket>())
	{
		LOG_DEBUG_CONSOLE("Attempting to listen to port ");

		m_listener.setBlocking(false);
		// Returns Done if opening the port was successful.
		if (m_listener.listen(s_port) != sf::Socket::Done)
		{
			LOG_DEBUG_CONSOLE("Failed to listen.");
		}
	}

	~Server() {}

	void Run()
	{
		while (m_connections.size() < s_minConnectedClients)
		{
			CheckConnectionRequests();
		}

		bool isRunning = true;
		while (isRunning)
		{
			m_networkHelper.SendMessages(&m_connections.front());
			m_networkHelper.ReceiveMessages(&m_connections.front());
		}
	}

private:
	void CheckConnectionRequests()
	{
		Common::Connection c(m_socket.get());
		sf::Socket::Status status = m_listener.accept(*m_socket);
		LOG_DEBUG_CONSOLE("status" + std::to_string(static_cast<int>(status)));
		if (status == sf::Socket::Done)
		{
			m_connections.push_back(std::move(c));
		}
	}

private:
	Common::NetworkHelper m_networkHelper;
	sf::TcpListener m_listener;
	std::vector<Common::Connection> m_connections;
	std::unique_ptr<sf::TcpSocket> m_socket;
};

//-----------------------------------------------------------------------------------

int main()
{
	Server server;
	server.Run();
	return 0;
}