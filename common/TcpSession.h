//---------------------------------------------------------------
//
// TcpSession.h
//

#pragma once

#include "common/NetworkTypes.h"

#include <asio.hpp>
#include <deque>
#include <memory>

namespace Common {

//===============================================================================

struct NetworkMessage;
class NetworkMessageParser;
class TcpSession : public std::enable_shared_from_this<TcpSession> {

public:
	TcpSession(asio::ip::tcp::socket socket, const std::string& loggingContext);

	~TcpSession();

	bool IsStopped() { return !m_socket.is_open(); }
	void Start();
	void Stop();
	void Write(std::string data);

private:
	void WaitWrite();
	void OnWaitWriteComplete(const std::error_code& ec);
	void WaitRead();
	void OnWaitReadComplete(const std::error_code& ec);
	void DoRead();
	void OnDoRead(const std::error_code ec, std::size_t bytesRead);
	void DoWrite();
	void OnDoWrite(const std::error_code& ec, uint32_t bytesWritten);

	// Indicates that the socket is ready to be written to.
	bool m_writeReady = false;

	// Filled with data over the wire before parsing.
	std::string m_inputBuffer;

	// Messages are written one at a time. Store a backlog of them.
	std::deque<std::string> m_outputBuffer;

	// Will parse messages that we get over the wire.
	std::unique_ptr<NetworkMessageParser> m_parser;

	// TODO: I think I should just send these out and not store them.
	// Parsed messages that need to be sent to be delivered to their destination.
	std::vector<NetworkMessage> m_messages;

	// The connected socket.
	asio::ip::tcp::socket m_socket;
};

//===============================================================================

} // namespace Common
