//---------------------------------------------------------------
//
// GameServer.h
//

#pragma once

#include "common/NetworkTypes.h"
#include "common/ThreadSafeQueue.h"

#include <atomic>
#include <memory>
#include <thread>

namespace Server {

//===============================================================================

class AsioEventProcessor;
class ClientSessionManager;
class ClientSessionEvents;
class Game;
class TcpListener;
class GameServer {

public:
	friend class ClientSessionManager;
	GameServer(Game* game);
	~GameServer();

	// Start the server and begin listening for and handling connections.
	void Start();

	// Stop the server. This will close all client connections.
	void Stop();

	// Returns true if the server is running.
	bool IsRunning() const { return m_isRunning; };

	// Posts a serialized message to the specified client.
	void PostMessageToClient(uint32_t clientId, const std::string& message);

	// Returns number of connected clients.
	uint32_t GetConnectionClientCount() const { return m_currentConnectionCount; }

	ClientSessionEvents& GetEvents();

private:
	// Helper class for managing client connection sessions.
	std::unique_ptr<ClientSessionManager> m_sessionManager;

	// Listens for connections.
	std::shared_ptr<TcpListener> m_listener;

	// Helper class that handles the asio work queue.
	std::unique_ptr<AsioEventProcessor> m_asioEventProcessor;

	// How many clients are currently connected.
	std::atomic<uint32_t> m_currentConnectionCount{0};
	
	// True if the server is currently accepting and handling connections.
	std::atomic<bool> m_isRunning;

	// Events related to client sessions.
	std::unique_ptr<ClientSessionEvents> m_events;

	// Pointer to parent.
	Game* m_game;
};

//===============================================================================

} // namespace GameServer

