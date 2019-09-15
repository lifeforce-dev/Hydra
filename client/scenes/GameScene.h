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

class Label;
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
	virtual void Render() override;

private:
	GameController* m_controller;
	std::unique_ptr<Label> m_label;
};

//===============================================================================

} // namespace Client
