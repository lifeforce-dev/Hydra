//---------------------------------------------------------------
//
// DebugController.cpp
//

#include "DebugController.h"

#include "client/DebugEvents.h"
#include "client/Game.h"

namespace Client {

//===============================================================================

void DebugController::SubscribeEvents()
{
	auto& events = g_game->GetDebugEvents();

	using namespace std::chrono;
	events.GetFullFrameCompletedEvent().subscribe(
		[this](const microseconds& frameTime)
	{
		// NYI
	});

	events.GetRenderFrameCompletedEvent().subscribe(
		[this](const microseconds& frameTime)
	{
		// NYI
	});

	events.GetEventHandlerFrameCompletedEvent().subscribe(
		[this](const microseconds& frameTime)
	{
		// NYI
	});

	events.GetSimFrameCompletedEvent().subscribe([this](const microseconds& frameTime)
	{
		// NYI
	});
}

//===============================================================================

} //namespace Client
