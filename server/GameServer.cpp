//---------------------------------------------------------------
//
// GameServer.cpp
//

#include "GameServer.h"
#include "common/AsioEventProcessor.h"
#include "common/Log.h"
#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"
#include "common/TcpSession.h"
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

class ClientTcpSession : public std::enable_shared_from_this<ClientTcpSession> {

public:
	ClientTcpSession(tcp::socket socket, uint32_t id)
		: m_session(std::make_shared<Common::TcpSession>(std::move(socket),
			"ServerClient-" + std::to_string(id)))
		, m_clientId(id)
	{
	}

	ClientTcpSession(const ClientTcpSession& other) = delete;
	bool operator==(ClientTcpSession& other)
	{
		return other.m_clientId == m_clientId;
	}

	uint32_t GetClientId() { return m_clientId; }
	std::shared_ptr<Common::TcpSession> GetSession() { return m_session; }

	// The current TcpSession for this client id.
	std::shared_ptr<Common::TcpSession> m_session;

	// Identifier for the client.
	uint32_t m_clientId = 0;
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
		auto newSession = std::make_shared<ClientTcpSession>(std::move(socket), clientId);
		newSession->GetSession()->Start();
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
			session->GetSession()->Stop();
		}
	}

	void DestroyAllSessions()
	{
		for (auto session : m_sessions)
		{
			if (session)
			{
				session->GetSession()->Stop();
			}
		}
	}

	const std::shared_ptr<ClientTcpSession> GetSessionById(uint32_t clientId) const
	{
		auto it = std::find_if(std::cbegin(m_sessions), std::cend(m_sessions),
			[clientId](const std::shared_ptr<ClientTcpSession>& session)
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
	std::set<std::shared_ptr<ClientTcpSession>> m_sessions;

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
	if (std::shared_ptr<ClientTcpSession> session = m_sessionManager->GetSessionById(clientId))
	{
		m_asioEventProcessor->Post([session, message{std::move(message)}]()
		{
			session->GetSession()->Write(message);
		});
	}
}

ClientSessionEvents& GameServer::GetEvents()
{
	return *m_events;
}

//===============================================================================

} // namespace GameServer
