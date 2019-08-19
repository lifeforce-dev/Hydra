//---------------------------------------------------------------
//
// TcpSession.cpp
//

#include "TcpSession.h"

#include "common/Log.h"
#include "common/NetworkMessageParser.h"

using tcp = asio::ip::tcp;

namespace Common {

//===============================================================================

namespace {
	std::shared_ptr<spdlog::logger> s_logger;
} // anon namespace

TcpSession::TcpSession(tcp::socket socket, const std::string& loggingContext)
	: m_socket(std::move(socket))
{
	REGISTER_LOGGER(loggingContext);
	s_logger = Log::Logger(loggingContext);

	// Reserve some space for read buffer.
	m_inputBuffer.reserve(32768);
	m_inputBuffer.resize(32768);

	// Reserve a good amount of space for queued messages.
	m_messages.reserve(1024);
}

TcpSession::~TcpSession()
{
}

void TcpSession::Start()
{
	SPDLOG_INFO("Client TCP session running...");
	WaitRead();
	WaitWrite();
}

void TcpSession::Stop()
{
	m_socket.close();
}

void TcpSession::Write(std::string data)
{
	m_outputBuffer.push_back(std::move(data));

	// Ensure that we're only processing one at a time. Begins async loop.
	if (m_writeReady && m_outputBuffer.size() == 1)
	{
		DoWrite();
	}
}

void TcpSession::WaitWrite()
{
	m_socket.async_wait(tcp::socket::wait_write,
		std::bind(&TcpSession::OnWaitWriteComplete,
			shared_from_this(),
			std::placeholders::_1));
}

void TcpSession::OnWaitWriteComplete(const std::error_code& ec)
{
	m_writeReady = true;
	if (!m_outputBuffer.empty())
	{
		DoWrite();
	}
}

void TcpSession::WaitRead()
{
	m_socket.async_wait(tcp::socket::wait_read,
		std::bind(&TcpSession::OnWaitReadComplete,
			shared_from_this(),
			std::placeholders::_1));
}

void TcpSession::OnWaitReadComplete(const std::error_code& ec)
{
	if (ec)
	{
		SPDLOG_LOGGER_ERROR(s_logger,
			"Error waiting for socket to be read ready. ec= {} ecc= {}",
			ec.value(), ec.category().name());
		return;
	}

	DoRead();
}

void TcpSession::DoRead()
{
	asio::async_read(m_socket,
		asio::buffer(m_inputBuffer,
			m_inputBuffer.size()),
		asio::transfer_at_least(2),
		std::bind(&TcpSession::OnDoRead,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void TcpSession::OnDoRead(const std::error_code ec, std::size_t bytesRead)
{

	if (IsStopped())
	{
		m_inputBuffer.clear();
		return;
	}

	if (ec)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Error reading data from server. ec= {}", ec.value());
		if (ec != asio::error::operation_aborted)
		{
			Stop();
		}
		m_inputBuffer.clear();
		return;
	}

	//if (bytesRead > 0)
	//{
	//	SPDLOG_LOGGER_INFO(s_logger, m_inputBuffer.substr(0, bytesRead));
	//}

	m_inputBuffer.clear();
}

void TcpSession::DoWrite()
{
	asio::async_write(m_socket, asio::buffer(m_outputBuffer.front().data(),
		m_outputBuffer.front().length()),
		std::bind(&TcpSession::OnDoWrite,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void TcpSession::OnDoWrite(const std::error_code& ec, uint32_t bytesWritten)
{
	if (ec)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Error writing data to server. ec= {}", ec.value());
		if (ec != asio::error::operation_aborted)
		{
			Stop();
		}
		return;
	}

	m_outputBuffer.pop_front();
	if (!m_outputBuffer.empty())
	{
		DoWrite();
	}
}

//===============================================================================

} // namespace Common
