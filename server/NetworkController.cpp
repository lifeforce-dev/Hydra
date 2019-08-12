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

class ServerMessageObserver : public Common::NetworkObserver
{
public:
	ServerMessageObserver(NetworkController* controller)
		: m_controller(controller)
	{

	}

	virtual void OnClientDisconnected(sf::TcpSocket* disconnectedSocket) override
	{
		// NYI
	}

	virtual void OnMessagesReceived(const std::vector<Common::NetworkMessage>& messages) override
	{
		// NYI
	}

private:
	NetworkController* m_controller;
};


NetworkController::NetworkController(GameServer* gameServer)
	: m_selector(std::make_unique<sf::SocketSelector>())
	, m_networkHelper(std::make_unique<Common::NetworkHelper>())
	, m_socketListener(std::make_unique<sf::TcpListener>())
	, m_freeSocket(std::make_unique<sf::TcpSocket>())
	, m_gameServer(gameServer)
	, m_messageObserver(std::make_unique<ServerMessageObserver>(this))
{
	m_socketListener->listen(s_port);
	m_selector->add(*m_socketListener);
	m_socketListener->setBlocking(false);

	m_networkHelper->AddObserver(m_messageObserver.get());
}

NetworkController::~NetworkController()
{
	m_networkHelper->RemoveObserver(m_messageObserver.get());
	if (m_listenThread.joinable())
	{
		m_listenThread.join();
	}
}

void NetworkController::BeginProcessNetworkConnections()
{
	m_listenThread = std::thread([this]()
		{
			while (true)
			{
				if (!m_selector->wait())
					return;

				if (m_selector->isReady(*m_socketListener))
				{
					m_gameServer->PostToMainThread([this]()
					{
						if (m_socketListener->accept(*m_freeSocket) == sf::Socket::Done)
						{
							m_selector->add(*m_freeSocket);
							m_clients.push_back(std::move(m_freeSocket));
							m_freeSocket = std::make_unique<sf::TcpSocket>();
						}
					});
				}
				else
				{
					m_gameServer->PostToMainThread([this]()
					{
						for (const auto& client : m_clients)
						{
							if (m_selector->isReady(*client))
							{
								m_networkHelper->ReceiveMessages(client.get());
							}
						}
					});
				}
			}
		});
}

void NetworkController::Process()
{
	ProcessMessages();
}

void NetworkController::ProcessMessages()
{
	// NYI
}

//===============================================================================

} // namespace Server
