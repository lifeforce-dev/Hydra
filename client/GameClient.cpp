//---------------------------------------------------------------
//
// GameClient.cpp
//

#include "client/GameClient.h"

#include "client/Game.h"
#include "common/AsioEventProcessor.h"
#include "common/Log.h"
#include "common/NetworkTypes.h"
#include "common/NetworkMessageParser.h"

using tcp = asio::ip::tcp;

namespace Client {

//===============================================================================

namespace {
	const std::string s_localIp = "127.0.0.1";
	const uint32_t s_port = 26000;
	const std::thread::id s_mainThreadId = std::this_thread::get_id();

	std::shared_ptr<spdlog::logger> s_logger;
}

class TcpSession : public std::enable_shared_from_this<TcpSession> {

public:
	TcpSession(tcp::socket socket)
		: m_socket(std::move(socket))
	{
		// Reserve a good amount of space for queued messages.
		m_messages.reserve(1024);

		WaitWrite();
	}

	~TcpSession()
	{
	}

	bool IsStopped() { return !m_socket.is_open(); }

	void Start()
	{
		SPDLOG_INFO("Client TCP session running...");
		WaitRead();
	}

	void Stop()
	{
		m_socket.close();
	}

	void Write(std::string data)
	{
		m_outputBuffer.push_back(std::move(data));

		// Ensure that we're only processing one at a time. Begins async loop.
		if (m_writeReady && m_outputBuffer.size() == 1)
		{
			DoWrite();
		}
	}

private:
	void WaitWrite()
	{
		m_socket.async_wait(tcp::socket::wait_write,
			std::bind(&TcpSession::OnWaitWriteComplete,
			shared_from_this(),
			std::placeholders::_1));
	}

	void OnWaitWriteComplete(const std::error_code& ec)
	{
		m_writeReady = true;
		DoWrite();
	}

	void WaitRead()
	{
		m_socket.async_wait(tcp::socket::wait_read,
			std::bind(&TcpSession::OnWaitReadComplete,
				shared_from_this(),
				std::placeholders::_1));
	}

	void OnWaitReadComplete(const std::error_code& ec)
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

	void DoRead()
	{
		asio::async_read(m_socket, asio::buffer(m_buffer),
			std::bind(&TcpSession::OnDoRead,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void OnDoRead(const std::error_code ec, std::size_t bytesRead)
	{
		if (IsStopped())
		{
			return;
		}

		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Error reading data from server. ec= {}", ec.value());
			if (ec != asio::error::operation_aborted)
			{
				Stop();
			}
			return;
		}
	}

	void DoWrite()
	{
		asio::async_write(m_socket, asio::buffer(m_outputBuffer.front().data(),
			m_outputBuffer.front().length()),
			std::bind(&TcpSession::OnDoWrite,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void OnDoWrite(const std::error_code& ec, uint32_t bytesWritten)
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

	// Indicates that the socket is ready to be written to.
	bool m_writeReady = false;

	// Filled with data over the wire before parsing.
	std::string m_buffer;

	// Messages are written one at a time. Store a backlog of them.
	std::deque<std::string> m_outputBuffer;

	// Will parse messages that we get over the wire.
	std::unique_ptr<Common::NetworkMessageParser> m_parser;

	// TODO: I think I should just send these out and not store them.
	// Parsed messages that need to be sent to be delivered to their destination.
	std::vector<Common::NetworkMessage> m_messages;

	// The connected socket.
	tcp::socket m_socket;
};

//-------------------------------------------------------------------------------

class TcpSessionConnector : public std::enable_shared_from_this<TcpSessionConnector> {

public:
	TcpSessionConnector(asio::io_context& ioc, GameClient* client)
		: m_socket(ioc)
		, m_client(client)
	{
		m_endpoints.emplace_back(asio::ip::make_address(s_localIp), s_port);
	}

	~TcpSessionConnector()
	{

	}

	bool IsConnected() { return m_isConnected; }

	const std::shared_ptr<TcpSession>& GetSession() { return m_session; }

	void DestroySession()
	{
		m_session->Stop();
	}

	void Connect()
	{
		SPDLOG_INFO("Attempting to connect client to server");
		DoConnect();
	}

private:
	void DoConnect()
	{
		asio::async_connect(m_socket, m_endpoints,
			std::bind(&TcpSessionConnector::OnDoConnect,
				shared_from_this(),
				std::placeholders::_1));
	}

	void OnDoConnect(const std::error_code& ec)
	{
		if (ec)
		{
			SPDLOG_ERROR("Error connecting to server. ec= {}", ec.value());

			// Server may not be running, keep trying.
			if (ec == asio::error::connection_refused)
			{
				m_socket.close();
				DoConnect();
			}
			return;
		}
		SPDLOG_INFO("Client connected to server.");
		m_isConnected = true;
		m_session = std::make_shared<TcpSession>(std::move(m_socket));
		m_session->Start();
	}

	// Empty after connected.
	tcp::socket m_socket;
	bool m_isConnected = false;

	std::vector<tcp::endpoint> m_endpoints;
	std::shared_ptr<TcpSession> m_session;

	GameClient* m_client;
};

//-------------------------------------------------------------------------------

GameClient::GameClient(Game* game)
	: m_asioEventProcessor(std::make_unique<Common::AsioEventProcessor>())
	, m_game(game)
{
	REGISTER_LOGGER("GameClient");
	s_logger = Log::Logger("GameClient");
}

GameClient::~GameClient()
{
}

void GameClient::Start()
{
	m_sessionConnector = std::make_shared<TcpSessionConnector>(m_asioEventProcessor->GetIoService(), this);
	m_sessionConnector->Connect();
	m_asioEventProcessor->Run();
}

void GameClient::Stop()
{
	m_sessionConnector->DestroySession();
}

void GameClient::PostMessageToServer(std::string message)
{
	m_asioEventProcessor->Post([session = m_sessionConnector->GetSession(),
		message{ std::move(message) }]()
	{
		session->Write(message);
	});
}

//===============================================================================

} // namespace Client
