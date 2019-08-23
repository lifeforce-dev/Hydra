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
#include "common/TcpSession.h"

using tcp = asio::ip::tcp;

namespace Client {

//===============================================================================

namespace {
	const std::string s_localIp = "127.0.0.1";
	const uint32_t s_port = 26000;
	const std::thread::id s_mainThreadId = std::this_thread::get_id();

	std::shared_ptr<spdlog::logger> s_logger;
}

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

	std::shared_ptr<Common::TcpSession> GetSession() { return m_session; }

	void DestroySession()
	{
		m_session->Stop();
	}

	void Connect()
	{
		SPDLOG_LOGGER_INFO(s_logger, "Attempting to connect client to server");
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
			SPDLOG_LOGGER_ERROR(s_logger, "Error connecting to server. ec= {}", ec.value());

			// Server may not be running, keep trying.
			if (ec == asio::error::connection_refused)
			{
				m_socket.close();
				DoConnect();
			}
			return;
		}

		SPDLOG_LOGGER_INFO(s_logger, "Client connected to server.");

		m_isConnected = true;
		m_session = std::make_shared<Common::TcpSession>(std::move(m_socket), "Client::GameClient");
		m_session->Start();
	}

	// Empty after connected.
	tcp::socket m_socket;
	bool m_isConnected = false;

	std::vector<tcp::endpoint> m_endpoints;
	std::shared_ptr<Common::TcpSession> m_session;

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
