//---------------------------------------------------------------
//
// GameClient.h
//

#pragma once

#include <memory>
#include <string>

namespace Common {
class AsioEventProcessor;
}

namespace Client {

//===============================================================================

class Game;
class TcpSessionConnector;
class TcpSession;
class GameClient {

friend class TcpSessionConnector;

public:
	GameClient(Game* controller);
	~GameClient();

	void Start();
	void Stop();
	void PostMessageToServer(std::string message);

	bool IsRunning() { return m_isRunning; }
	bool IsConnected();;


private:
	std::unique_ptr<Common::AsioEventProcessor> m_asioEventProcessor;
	std::shared_ptr<TcpSessionConnector> m_sessionConnector;

	Game* m_game;
	bool m_isRunning = false;
	bool m_isConnected = false;
};

//===============================================================================

} // namespace Client
