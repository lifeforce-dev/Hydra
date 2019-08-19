//---------------------------------------------------------------
//
// GameServer.cpp
//

#include "GameServer.h"
#include "common/AsioEventProcessor.h"
#include "common/Log.h"
#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"
#include "server/ClientSessionEvents.h"
#include "server/Game.h"

#include <asio.hpp>
#include <functional>
#include <memory>
#include <set>
#include <system_error>

using tcp = asio::ip::tcp;

namespace Server {

//===============================================================================

namespace {

const char* s_localIp = "127.0.0.1";
const uint16_t s_port = 26000;
const std::thread::id s_mainThreadId = std::this_thread::get_id();

std::shared_ptr<spdlog::logger> s_logger;

} // anon namespace

//-------------------------------------------------------------------------------

class TcpSession : public std::enable_shared_from_this<TcpSession> {

public:
	TcpSession(tcp::socket socket, uint32_t id)
		: m_socket(std::move(socket))
		, m_clientId(id)
		, m_parser(std::make_unique<Common::NetworkMessageParser>())
	{
		m_buffer.reserve(32768);
		m_buffer.resize(32768);
	}

	TcpSession(const TcpSession& other) = delete;
	bool operator==(TcpSession& other)
	{
		return other.m_clientId == m_clientId;
	}

	bool IsStopped() const { return !m_socket.is_open(); }

	void Start()
	{
		SPDLOG_LOGGER_INFO(s_logger, "Created session. id={}", m_clientId);
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
			DoWrite(data);
		}
	}

	uint32_t GetClientId() { return m_clientId; }

private:
	void DoRead()
	{
		asio::async_read(m_socket,
			asio::buffer(m_buffer,
			m_buffer.size()),
			asio::transfer_at_least(56),
			std::bind(&TcpSession::OnDoRead,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void OnDoRead(const std::error_code& ec, std::size_t bytesRead)
	{
		if (IsStopped())
			return;

		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Error readiung data from client. ec= {} ecc= {} bytes written= {}",
				ec.value(), ec.category().name(), bytesRead);
			if (ec != asio::error::operation_aborted)
			{
				Stop();
			}
			return;
		}

		// TODO
		//m_parser->ExtractMessages(m_buffer, m_messages);
		//HandleMessages();

		DoRead();
	}

	void HandleMessages()
	{
		// NYI
		m_messages.clear();
	}

	void DoWrite(const std::string& data)
	{
		asio::async_write(m_socket, asio::buffer(std::move(data)),
			std::bind(&TcpSession::OnDoWrite,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void OnDoWrite(const std::error_code& ec, uint32_t bytesWritten)
	{
		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Error writing data. ec= {} ecc= {} bytes written= {}",
				ec.value(), ec.category().name(), bytesWritten);

			if (ec != asio::error::operation_aborted)
			{
				Stop();
			}
		}
	}

	// Indicates that the socket is ready to be written to.
	bool m_writeReady = false;

	// Messages are written one at a time. Store a backlog of them.
	std::deque<std::string> m_outputBuffer;

	// Identifier for the client.
	uint32_t m_clientId = 0;

	// The connected socket.
	tcp::socket m_socket;

	// Filled with data over the wire before parsing.
	std::string m_buffer;

	// Will parse messages that we get over the wire.
	std::unique_ptr<Common::NetworkMessageParser> m_parser;

	// Parsed messages from over the wire.
	std::vector<Common::NetworkMessage> m_messages;
};

//-------------------------------------------------------------------------------

class ClientSessionManager
{
public:
	ClientSessionManager(GameServer* server)
	: m_server(server)
	{
	}

	~ClientSessionManager() {}

	void CreateSession(tcp::socket socket)
	{
		uint32_t clientId = m_nextId++;
		auto newSession = std::make_shared<TcpSession>(std::move(socket), clientId);
		newSession->Start();
		m_sessions.insert(newSession);

		m_server->m_game->PostToMainThread([this, clientId]()
		{
			m_server->GetEvents().GetSessionCreatedEvent().notify(clientId);
		});
	}

	void DestroySession(uint32_t clientId)
	{
		auto session = GetSessionById(clientId);
		if (session)
		{
			session->Stop();
		}
	}

	void DestroyAllSessions()
	{
		for (auto session : m_sessions)
		{
			if (session)
			{
				session->Stop();
			}
		}
	}

	const std::shared_ptr<TcpSession> GetSessionById(uint32_t clientId) const
	{
		auto it = std::find_if(std::cbegin(m_sessions), std::cend(m_sessions),
			[clientId](const std::shared_ptr<TcpSession>& session)
			{
				return session->GetClientId() == clientId;
			});

		if (it != std::cend(m_sessions))
		{
			return *it;
		}

		SPDLOG_LOGGER_ERROR(s_logger, "Tried to get session that doesn't exist. clientId= {}",
			clientId);
		return nullptr;
	}

	// Increments every time a client is connected, guaranteeing uniqueness.
	std::atomic<uint32_t> m_nextId{ 0 };

	// Store sessions here.
	std::set<std::shared_ptr<TcpSession>> m_sessions;

	// Pointer to parent.
	GameServer* m_server;
};

//-------------------------------------------------------------------------------

class TcpListener : public std::enable_shared_from_this<TcpListener> {

public:
	TcpListener(asio::io_context& ioc,
		ClientSessionManager* connectionManager)
		: m_acceptor(ioc)
		, m_socket(ioc)
		, m_connectionManager(connectionManager)
	{
	}

	void Run()
	{
		std::error_code ec;

		tcp::endpoint endPoint{ asio::ip::make_address(s_localIp), s_port };
		SPDLOG_LOGGER_INFO(s_logger, "Attempting to listen on port{}", s_port);

		tcp::acceptor acceptor{ m_socket.get_io_context() };

		acceptor.open(endPoint.protocol(), ec);
		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger,"Error opening acceptor. ec={}",
				ec.value());
			return;
		}

		acceptor.bind(endPoint, ec);
		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Error binding acceptor to endpoint. ec={}",
				ec.value());
			return;
		}
			
		acceptor.listen(asio::socket_base::max_listen_connections, ec);
		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Error listening for connections. ec={}",
				ec.value());
			return;
		}

		asio::socket_base::reuse_address option(true);
		acceptor.set_option(option);

		m_acceptor = std::move(acceptor);

		if (!m_acceptor.is_open())
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to find available port. ec={}",
				ec.value());
			return;
		}

		m_isRunning = true;
		m_port = m_acceptor.local_endpoint(ec).port();
		SPDLOG_LOGGER_INFO(s_logger, "Listening on port {}", m_port);
		DoAccept();
	}

private:
	void OnAccept(const std::error_code& ec)
	{
		if (ec)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Error accepting incoming connection. ec={}",
				ec.value());
		}
		else
		{
			SPDLOG_LOGGER_INFO(s_logger, "Socket accepted. Creating session.");

			// create session
			m_connectionManager->CreateSession(std::move(m_socket));
		}

		if (m_isRunning)
		{
			DoAccept();
		}
	}

	void DoAccept()
	{
		m_acceptor.async_accept(
			m_socket,
			std::bind(&TcpListener::OnAccept, shared_from_this(), std::placeholders::_1));
	}

private:
	tcp::acceptor m_acceptor;
	tcp::socket m_socket;
	ClientSessionManager* m_connectionManager;

	uint16_t m_port = 0;
	bool m_isRunning = false;
};

GameServer::GameServer(Game* game)
	: m_asioEventProcessor(std::make_unique<Common::AsioEventProcessor>())
	, m_sessionManager(std::make_unique<ClientSessionManager>(this))
	, m_events(std::make_unique<ClientSessionEvents>())
	, m_game(game)
{
	REGISTER_LOGGER("Server");
	s_logger = Log::Logger("Server");
}

//-------------------------------------------------------------------------------

void GameServer::Start()
{
	m_listener = std::make_shared<TcpListener>(m_asioEventProcessor->GetIoService(),
		m_sessionManager.get());
	m_listener->Run();

	m_asioEventProcessor->Run();
}

GameServer::~GameServer()
{
}

void GameServer::Stop()
{
	m_sessionManager->DestroyAllSessions();
}

void GameServer::PostMessageToClient(uint32_t clientId, std::string message)
{
	if (std::shared_ptr<TcpSession> session = m_sessionManager->GetSessionById(clientId))
	{
		m_asioEventProcessor->Post([session, message{std::move(message)}]()
		{
			session->Write(message);
		});
	}
}

ClientSessionEvents& GameServer::GetEvents()
{
	return *m_events;
}

//===============================================================================

} // namespace GameServer
