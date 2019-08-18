//---------------------------------------------------------------
//
// ClientSessionEvents.cpp
//

#pragma once

#include <observable.hpp>

namespace Server {

//===============================================================================

class ClientSessionEvents {
public:
	using SessionCreatedEvent = observable::subject<void(uint32_t clientId)>;
	SessionCreatedEvent& GetSessionCreatedEvent() { return m_sessionCreatedEvent; }

	using SessionDestroyedEvent = observable::subject<void(uint32_t clientId)>;
	SessionDestroyedEvent& GetSessionDestroyedEvent() { return m_sessionDestroyedEvent; }

private:
	SessionCreatedEvent m_sessionCreatedEvent;
	SessionDestroyedEvent m_sessionDestroyedEvent;
};

//===============================================================================

} // namespace Common
