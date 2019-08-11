//---------------------------------------------------------------
//
// NetworkController.cpp
//

#include "client/NetworkController.h"

#include "client/Game.h"
#include "client/network/ClientTcpSocket.h"
#include "common/Log.h"
#include "common/NetworkHelper.h"
#include "common/NetworkTypes.h"

#include <windows.h>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

namespace Client {

namespace {

	const sf::Time s_connectionTimeout = sf::milliseconds(1000);
}

//===============================================================================

NetworkController::NetworkController(Game* game)
	: m_networkHelper(std::make_unique<Common::NetworkHelper>())
	, m_socket(std::make_unique<ClientTcpSocket>())
	, m_connection(std::make_unique<Common::Connection>(m_socket.get()))
	, m_retryTimer(std::make_unique<sf::Clock>())
	, m_game(game)
{
}

NetworkController::~NetworkController()
{
}

void NetworkController::ConnectToServer(const std::string& address, int port)
{
	m_socket->Connect();
}

void NetworkController::SendMessageToServer(Common::MessageId type, const std::string& message)
{
	m_networkHelper->QueueMessage(type, message);
}

bool NetworkController::IsConnected() const
{
	if (m_socket->IsConnected())
	{
		return true;
	}
	else
	{
		auto time = m_retryTimer->getElapsedTime();
		if (time >= sf::seconds(5))
		{
			m_socket->Connect();
			m_retryTimer->restart();
		}
	}
	return false;
}

void NetworkController::OnConnectedToServer()
{
	const Common::Connection& connection = *m_connection.get();
	LOG_DEBUG("Connection attempt succeeded."
		" remote-ip=" + connection.socket->getRemoteAddress().toString() +
		" remote-port=" + std::to_string(connection.socket->getRemotePort()) +
		" local-port=" + std::to_string(connection.socket->getLocalPort()));
}

void NetworkController::UpdateConnectionStatus()
{
	bool isconnected = IsConnected();
	if (isconnected != m_isConnected)
	{
		m_isConnected = isconnected;
		OnConnectedToServer();
	}
}

void NetworkController::Process()
{
	UpdateConnectionStatus();
	if (m_isConnected)
	{
		m_networkHelper->SendMessages(m_connection.get());
		m_networkHelper->ReceiveMessages(m_connection.get());
	}
}

//===============================================================================

} // namespace Client
