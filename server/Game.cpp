//---------------------------------------------------------------
//
// Game.cpp
//

#include "server/Game.h"

#include "common/Log.h"
#include "server/ClientSessionEvents.h"
#include "server/GameClient.h"
#include "server/GameServer.h"

#include <assert.h>
#include <thread>

namespace Server {

//===============================================================================

namespace {

std::shared_ptr<spdlog::logger> s_logger;
const std::thread::id s_mainThreadId = std::this_thread::get_id();

} // anon namespace

Game::Game()
	: m_server(std::make_unique<GameServer>(this))
{
	auto& events = m_server->GetEvents();
	events.GetSessionCreatedEvent().subscribe(
		[this](uint32_t clientId)
		{
			assert(std::this_thread::get_id() == s_mainThreadId);
			SPDLOG_LOGGER_INFO(s_logger, "OnSessionCreatedEvent clientId= {}", clientId);

			m_clients.emplace_back(this, clientId);
		});

	REGISTER_LOGGER("Server::Game");
	s_logger = Log::Logger("Server::Game");
	m_server->Start();
}

Game::~Game()
{
}

void Game::Run()
{
	while (m_isRunning)
	{
		ProcessCallbackQueue();
		for (auto& c : m_clients)
		{
			c->Process();
		}
	}
}

void Game::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

void Game::ProcessCallbackQueue()
{
	m_callbackQueue.SwapWithEmpty(m_processCbQueue);

	for (auto cb : m_processCbQueue)
	{
		if (cb)
		{
			cb();
		}
	}

	if (!m_processCbQueue.empty())
	{
		std::deque<std::function<void()>>().swap(m_processCbQueue);
	}
}

//===============================================================================

} // namespace Server
