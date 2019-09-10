//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "client/scenes/Scene.h"

#include <memory>

namespace Common {
	class Connection;
	class NetworkHelper;
}

namespace Client {

//===============================================================================

class NetworkController;
class GameController;

class GameScene : public Scene
{
public:
	GameScene(GameController* gameController);
	~GameScene();

	// Scene impl.

	// Will pump all event queues here.
	virtual void ProcessEvents() override;

	// Updates resulting from the event processing will be handled here.
	virtual void Update() override;

	// Called in the event loop.
	virtual void Draw() override;

private:
	GameController* m_application;
};

//===============================================================================

} // namespace Client
