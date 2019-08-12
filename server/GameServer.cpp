//---------------------------------------------------------------
//
// GameServer.cpp
//

#include "server/GameServer.h"

#include "server/NetworkController.h"

namespace Server {

//===============================================================================

GameServer::GameServer()
	: m_networkController(std::make_unique<NetworkController>(this))
{

}

GameServer::~GameServer()
{

}

void GameServer::Run()
{
	m_networkController->BeginProcessNetworkConnections();

	while (m_isRunning)
	{
		ProcessCallbackQueue();
		m_networkController->Process();
	}
}

void GameServer::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

void GameServer::ProcessCallbackQueue()
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
