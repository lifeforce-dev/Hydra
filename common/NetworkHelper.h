//---------------------------------------------------------------
//
// NetworkHelper.h
//

#pragma once

#include "common/NetworkTypes.h"
#include "common/Observatory.h"

#include <memory>
#include <deque>
#include <vector>
#include <utility>

namespace sf {
	class IpAddress;
	class TcpSocket;
}

namespace Common {

//===============================================================================

enum struct MessageType : uint32_t;
class NetworkMessageParser;
struct NetworkMessage;

class NetworkHelper : public Observatory<NetworkObserver>
{
public:
	NetworkHelper();
	~NetworkHelper();

	// Called in the event loop. Will send all queued messages.
	void SendMessages(sf::TcpSocket* socket);

	// Called in the event loop. Will receive up to a certain amount of bytes per call.
	void ReceiveMessages(sf::TcpSocket* socket);

	// Queues a message to be sent the next time SendMessages is called. The type is needed so it can be
	// packaged with the message.
	void QueueMessage(MessageId type, const std::string& message);

	void HandleMessages(const std::vector<NetworkMessage>& messages);

private:
	std::unique_ptr<NetworkMessageParser> m_parser;
	std::deque<std::pair<MessageHeader, std::string>> m_messageQueue;
};

//===============================================================================

} // namespace Common
