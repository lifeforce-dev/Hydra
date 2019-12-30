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
	RenderEngineEvents() = default;
	~RenderEngineEvents() = default;

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

	using MainWindowCreatedEvent = observable::subject<void(SDL_Window* window)>;
	MainWindowCreatedEvent& GetMainWindowCreatedEvent() { return m_mainWindowCreatedEvent; }

	using MainWindowAboutToBeDestroyedEvent = observable::subject<void(SDL_Window * window)>;
	MainWindowAboutToBeDestroyedEvent& GetMainWindowAboutToBeDestroyedEvent()
	{ return m_mainWindowAboutToBeDestroyedEvent; }

	using MainWindowDestroyedEvent = observable::subject<void(SDL_Window * window)>;
	MainWindowDestroyedEvent& GetMainWindowDestroyedEvent() { return m_mainWindowDestroyedEvent; }

private:
	// Views
	ViewCreatedEvent m_viewCreatedEvent;
	ViewAboutToBeDestroyedEvent m_viewAboutToBeDestroyedEvent;
	ViewDestroyedEvent m_viewDestroyedEvent;

	// MainWindow
	MainWindowSizeChangedEvent m_mainWindowSizeChangedEvent;
	MainWindowExposedEvent m_mainWindowExposedEvent;
	MainWindowCreatedEvent m_mainWindowCreatedEvent;
	MainWindowDestroyedEvent m_mainWindowDestroyedEvent;
	MainWindowAboutToBeDestroyedEvent m_mainWindowAboutToBeDestroyedEvent;
};

//===============================================================================

} // namespace Client
