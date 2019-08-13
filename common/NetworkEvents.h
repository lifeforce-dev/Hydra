//---------------------------------------------------------------
//
// NetworkEvents.h
//

#pragma once

#include <observable.hpp>

namespace Common {
class NetworkEvents {
public:
	using ClientDisconnectedEvent = observable::subject<void()>;
	ClientDisconnectedEvent& GetClientDisconnectEvent() { return m_clientDiconnectEvent; }

	ClientDisconnectedEvent m_clientDiconnectEvent;
};

}
