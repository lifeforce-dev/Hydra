//---------------------------------------------------------------
//
// ClientTcpSocket.cpp
//

#include "ClientTcpSocket.h"

#include "common/Log.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <windows.h>
#include <string>

namespace Client {

//===============================================================================

namespace {
	const int s_port = 50001;
	const std::string s_serverAddress = "127.0.0.1";
	const sf::Time s_connectionTimeout = sf::milliseconds(500);
	const sf::Time s_connectionPollTime = sf::milliseconds(1);

} // anon namespace

ClientTcpSocket::ClientTcpSocket()
{
	setBlocking(false);
}

bool ClientTcpSocket::IsConnected()
{
	// Setup the selector
	fd_set selector;
	FD_ZERO(&selector);
	FD_SET(getHandle(), &selector);

	// Setup the timeout
	timeval time;
	time.tv_sec = static_cast<long>(s_connectionPollTime.asMicroseconds() / 1000000);
	time.tv_usec = static_cast<long>(s_connectionPollTime.asMicroseconds() % 1000000);

	// First param is ignored.
	int count = select(NULL, NULL, &selector, NULL, &time);
	return count > 0;
}


void ClientTcpSocket::Connect()
{
	connect(s_serverAddress, s_port);
	if (int ec = WSAGetLastError())
	{
		if (ec == WSAEWOULDBLOCK)
		{
			LOG_DEBUG("Connecting to server...");
		}
		else if (ec == WSAEALREADY)
		{
			LOG_DEBUG("Error: operation already in progress...");
		}
		else if (ec == WSAEISCONN)
		{
			LOG_DEBUG("Error: Already connected. error=" + std::to_string(ec));
		}
		else if (ec == WSAENETUNREACH)
		{
			LOG_DEBUG("Error: Network unreachable error=" + std::to_string(ec));
		}
		else if (ec == WSAETIMEDOUT)
		{
			LOG_DEBUG("Error: Network connection attempt timed out. error=" + std::to_string(ec));
		}
		else if (ec == WSAECONNREFUSED)
		{
			LOG_DEBUG("Error: Network connection attempt refused. error=" + std::to_string(ec));
		}
		else
		{
			LOG_DEBUG("Error: There was an error attempting to connect to server. error="
				+ std::to_string(ec));
		}
	}
}

//===============================================================================

} // namespace Client
