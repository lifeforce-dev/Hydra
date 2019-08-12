//---------------------------------------------------------------
//
// ClientTcpSocket.h
//

#pragma once

#include <memory>

#include <SFML/Network/TcpSocket.hpp>

namespace sf {
	class Clock;
}

namespace Client {

//===============================================================================

class ClientTcpSocket : public sf::TcpSocket
{
public:
	ClientTcpSocket();
	bool IsConnected();
	void Connect();
};

//===============================================================================

} // namespace Client
