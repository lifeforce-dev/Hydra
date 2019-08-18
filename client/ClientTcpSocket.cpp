//---------------------------------------------------------------
//
// ClientTcpSocket.cpp
//

#include "ClientTcpSocket.h"

#include <windows.h>

#include "common/Log.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <string>

namespace Client {

//===============================================================================

namespace {
	const int s_port = 26000;
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
}

//===============================================================================

} // namespace Client
