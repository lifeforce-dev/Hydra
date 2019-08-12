//---------------------------------------------------------------
//
// ByteBuffer.h
//

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace sf {
	class TcpSocket;
}

namespace Common {

//===============================================================================

enum struct MessageId : uint32_t
{
	None,
	Creature,
	Attack,
	Move,
	TestMessage,
};

struct MessageHeader
{
	void Clear()
	{
		messageType = MessageId::None;
		messageLength = 0;
	};

	MessageId messageType = MessageId::None;
	uint32_t messageLength = 0;
};

struct NetworkMessage
{
	NetworkMessage() {}
	NetworkMessage(NetworkMessage&& m)
		: header(std::move(m.header))
		, messageData(std::move(m.messageData))
	{
	}

	NetworkMessage(const MessageHeader& h)
		: header(h)
	{
	}

	MessageHeader header;
	std::string messageData;
};

class NetworkObserver {

public:
	virtual void OnClientDisconnected(sf::TcpSocket* disconnectedSocket) = 0 {}
	virtual void OnMessagesReceived(const std::vector<NetworkMessage>& messages) = 0 {}
};


//===============================================================================

} // namespace Common
