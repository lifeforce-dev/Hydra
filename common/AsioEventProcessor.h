//---------------------------------------------------------------
//
// AsioEventProcessor.h
//

#pragma once

#include <asio.hpp>

namespace Common {

class AsioEventProcessor
{
public:
	AsioEventProcessor();
	~AsioEventProcessor();

	// Returns the asio::io_service that's servicing the queue.
	asio::io_service& GetIoService();

	// Post work to the io_service to be processed asynchronously.
	void Post(const std::function<void()>& cb);

	// Kicks off a new thread and starts the service on it.
	void Run();

private:
	void DoRun();

private:
	std::unique_ptr<asio::io_service> m_ios;
	std::unique_ptr<asio::io_service::work> m_work;
	std::thread m_thread;
	bool m_isReady = false;
};

} // namespace common
