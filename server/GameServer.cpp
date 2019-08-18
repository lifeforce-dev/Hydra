//---------------------------------------------------------------
//
// GameServer.cpp
//

#include "GameServer.h"
#include "common/Log.h"
#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"
#include "server/ClientSessionEvents.h"

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

	bool IsStopped() const { return !m_socket.is_open(); }

	void Start()
	{
		Logger::ServerLogger().info("Created session. id={}", m_clientId);
		DoRead();
	}

	void Stop()
	{
		m_socket.close();
	}

	void Write(const std::string& data)
	{
		if (m_socket.is_open())
		{
			DoWrite(data);
		}
	}

	uint32_t GetClientId() { return m_clientId; }

private:
	void DoRead()
	{
		asio::async_read(m_socket, asio::buffer(m_buffer),
			std::bind(&TcpSession::OnDoRead,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void OnDoRead(const std::error_code& ec, std::size_t bytesTransferred)
	{
		if (IsStopped())
			return;

		if (ec)
		{
			Logger::ServerLogger().error("Error reading data. clientId= {} ec= {}",
				m_clientId, ec.value());
			if (ec != asio::error::operation_aborted)
			{
				Stop();
			}
			return;
		}

		m_parser->ExtractMessages(m_buffer, m_messages);
		HandleMessages();

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
			Logger::ServerLogger().error("Error writing data. ec= {} bytes written= {}",
				ec.value(), bytesWritten);

			if (ec != asio::error::operation_aborted)
			{
				Stop();
			}
		}
	}

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
		std::make_shared<TcpSession>(std::move(socket), clientId)->Start();
		m_server->GetEvents().GetSessionCreatedEvent().notify(clientId);
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

		Logger::ServerLogger().error("Tried to get session that doesn't exist. clientId= {}", clientId);
		return nullptr;
	}

	// Increments every time a client is connected, guaranteeing uniqueness.
	std::atomic<uint32_t> m_nextId{ 0 };

	// Store sessions here.
	std::vector<std::shared_ptr<TcpSession>> m_sessions;

	// Pointer to parent.
	GameServer* m_server;
};

//-------------------------------------------------------------------------------

class AsioEventProcessor
{
public:
	AsioEventProcessor()
		: m_ios(std::make_unique<asio::io_service>())
	{
		m_work.reset(new asio::io_service::work(*m_ios));
		m_ios->post([this]()
			{
				m_isReady = true;
			});
	}

	~AsioEventProcessor()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	asio::io_service& GetIoService()
	{
		return *m_ios;
	}

	void Post(const std::function<void()>& cb)
	{
		m_ios->post(cb);
	}

	void Run()
	{
		m_thread = std::thread(([this]() { DoRun(); }));
	}

private:
	void DoRun()
	{
		std::error_code ec;
		m_ios->run(ec);

		if (ec)
		{
			Logger::ServerLogger().error("ASIO error occurred while running a task. ec={}", ec.value());
		}
	}

private:
	std::unique_ptr<asio::io_service> m_ios;
	std::unique_ptr<asio::io_service::work> m_work;
	std::thread m_thread;
	bool m_isReady = false;
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
		Logger::ServerLogger().info("Attempting to listen on port{}", s_port);

		tcp::acceptor acceptor{ m_socket.get_io_context() };

		acceptor.open(endPoint.protocol(), ec);
		if (ec)
		{
			Logger::ServerLogger().error("Error opening acceptor. ec={}", ec.value());
			return;
		}

		acceptor.bind(endPoint, ec);
		if (ec)
		{
			Logger::ServerLogger().error("Error binding acceptor to endpoint. ec={}", ec.value());
			return;
		}
			
		acceptor.listen(asio::socket_base::max_listen_connections, ec);
		if (ec)
		{
			Logger::ServerLogger().error("Error listening for connections. ec={}", ec.value());
			return;
		}

		m_acceptor = std::move(acceptor);

		if (!m_acceptor.is_open())
		{
			Logger::ServerLogger().error("Failed to find available port. ec={}", ec.value());
			return;
		}

		m_isRunning = true;
		m_port = m_acceptor.local_endpoint(ec).port();
		Logger::ServerLogger().info("Listening on port {}", m_port);
		DoAccept();
	}

private:
	void OnAccept(const std::error_code& ec)
	{
		if (ec)
		{
			Logger::ServerLogger().error("Error accepting incoming connection. ec={}", ec.value());
		}
		else
		{
			// create session
			Logger::ServerLogger().info("Socket accepted. Creating session.");

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

GameServer::GameServer()
	: m_asioEventProcessor(std::make_unique<AsioEventProcessor>())
	, m_sessionManager(std::make_unique<ClientSessionManager>(this))
	, m_events(std::make_unique<ClientSessionEvents>())
{
}

//-------------------------------------------------------------------------------

void GameServer::Start()
{
	m_listener = std::make_shared<TcpListener>(m_asioEventProcessor->GetIoService(), m_sessionManager.get());
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

void GameServer::PostMessageToClient(uint32_t clientId, const std::string& message)
{
	if (const std::shared_ptr<TcpSession>& session = std::move(m_sessionManager->GetSessionById(clientId)))
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
