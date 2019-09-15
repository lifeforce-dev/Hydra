//---------------------------------------------------------------
//
// Scene.h
//

#pragma once

namespace Client {

//===============================================================================

class Scene
{
public:
	// Handle all events in here (network, user input, etc).
	virtual void ProcessEvents() = 0;

	// Handle updates that would result from handling events in here.
	virtual void Update() = 0;

	// Render loop.
	virtual void Render() = 0;
};

//===============================================================================

} // namespace Client
