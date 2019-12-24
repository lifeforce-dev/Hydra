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

	using MainWindowSizeChangedEvent = observable::subject<void()>;
	MainWindowSizeChangedEvent& GetMainWindowSizeChangedEvent() { return m_mainWindowSizeChangedEvent; }

	using MainWindowExposedEvent = observable::subject<void()>;
	MainWindowExposedEvent& GetMainWindowExposedEvent() { return m_mainWindowExposedEvent; }

private:
	ViewCreatedEvent m_viewCreatedEvent;
	ViewAboutToBeDestroyedEvent m_viewAboutToBeDestroyedEvent;
	ViewDestroyedEvent m_viewDestroyedEvent;
	MainWindowSizeChangedEvent m_mainWindowSizeChangedEvent;
	MainWindowExposedEvent m_mainWindowExposedEvent;
};

//===============================================================================

} // namespace Client
