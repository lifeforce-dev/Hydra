//---------------------------------------------------------------
//
// NetworkController.cpp
//

#include "server/NetworkController.h"

#include "common/Log.h"
#include "common/NetworkHelper.h"
#include "common/NetworkTypes.h"
#include "server/GameServer.h"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

namespace Server {

//===============================================================================
namespace {
	const uint32_t s_port = 50001;
} // anon namespace


NetworkController::NetworkController(GameServer* gameServer)
	: m_selector(std::make_unique<sf::SocketSelector>())
	, m_networkHelper(std::make_unique<Common::NetworkHelper>())
	, m_socketListener(std::make_unique<sf::TcpListener>())
	, m_freeSocket(std::make_unique<sf::TcpSocket>())
	, m_gameServer(gameServer)
{
	m_socketListener->listen(s_port);
	m_selector->add(*m_socketListener);
	m_socketListener->setBlocking(false);
}

NetworkController::~NetworkController()
{
	if (m_listenThread.joinable())
	{
		m_listenThread.join();
	}
}

void NetworkController::BeginAcceptingConnections()
{
	m_listenThread = std::thread([this]()
		{
			while (true)
			{
				if (!m_selector->wait())
					return;

				if (!m_selector->isReady(*m_socketListener))
					return;

				m_gameServer->PostToMainThread([this]()
					{
						if (m_socketListener->accept(*m_freeSocket) == sf::Socket::Done)
						{
							m_selector->add(*m_freeSocket);
							m_sockets.push_back(std::move(m_freeSocket));
							m_freeSocket = std::make_unique<sf::TcpSocket>();
						}
					});
			}
		});
}

void NetworkController::Process()
{
	
}

//===============================================================================

} // namespace Server
