//---------------------------------------------------------------
//
// RenderEngineEvents.h
//

#pragma once
#include "client/View.h"

#include <cstdint>
#include <observable.hpp>

namespace Client {

//===============================================================================

class RenderEngineEvents {
public:
	using ViewCreatedEvent = observable::subject<void(View* view)>;
	ViewCreatedEvent& GetViewCreatedEvent() { return m_viewCreatedEvent; }

	using ViewAboutToBeDestroyedEvent = observable::subject<void(uint32_t id)>;
	ViewAboutToBeDestroyedEvent& GetViewAboutToBeDestroyedEvent() { return m_viewAboutToBeDestroyedEvent; }

	using ViewDestroyedEvent = observable::subject<void(uint32_t id)>;
	ViewDestroyedEvent& GetViewDestroyedEvent() { return m_viewDestroyedEvent; }

private:
	ViewCreatedEvent m_viewCreatedEvent;
	ViewAboutToBeDestroyedEvent m_viewAboutToBeDestroyedEvent;
	ViewDestroyedEvent m_viewDestroyedEvent;
};

//===============================================================================

} // namespace Client
