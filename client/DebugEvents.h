//---------------------------------------------------------------
//
// DebugEvents.h
//

#pragma once

#include <chrono>
#include <observable.hpp>

namespace Client {

//===============================================================================

class DebugEvents
{
public:
	DebugEvents() = default;
	~DebugEvents() = default;

	using SimFrameCompletedEvent = observable::subject<void(const std::chrono::microseconds&)>;
	SimFrameCompletedEvent& GetSimFrameCompletedEvent() { return m_simFrameCompletedEvent; }

	using RenderFrameCompletedEvent = observable::subject<void(const std::chrono::microseconds&)>;
	RenderFrameCompletedEvent& GetRenderFrameCompletedEvent() { return m_renderFrameCompletedEvent; }
	
	using EventHandlerFrameCompleted = observable::subject<void(const std::chrono::microseconds&)>;
	EventHandlerFrameCompleted& GetEventHandlerFrameCompletedEvent() { return m_eventHandleCompletedEvent; }

	using FullFrameCompletedEvent = observable::subject<void(const std::chrono::microseconds&)>;
	FullFrameCompletedEvent& GetFullFrameCompletedEvent() { return m_fullFrameCompletedEvent; }

private:
	SimFrameCompletedEvent m_simFrameCompletedEvent;
	RenderFrameCompletedEvent m_renderFrameCompletedEvent;
	EventHandlerFrameCompleted m_eventHandleCompletedEvent;
	FullFrameCompletedEvent m_fullFrameCompletedEvent;
};

//===============================================================================

} // namespace Client
