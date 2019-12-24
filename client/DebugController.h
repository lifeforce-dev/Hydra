//---------------------------------------------------------------
//
// DebugController.h
//

#pragma once

namespace Client {

//===============================================================================

struct PerformanceData
{
	// How long it took to render this frame.
	int renderDurationSeconds = 0;

	// How long it took to process simulation updates this frame.
	int simDurationSeconds = 0;

	// How long it took to process all events for this frame.
	int eventHandleDurationSeconds = 0;

	// How long it took to process an entire frame.
	int fullFrameTime = 0;
};

class DebugController
{
public:
	DebugController() = default;
	~DebugController() = default;
	void SubscribeEvents();

private:
	PerformanceData m_windowData;
};

//===============================================================================

} // namespace Client
