//---------------------------------------------------------------
//
// AsioEventProcessor.cpp
//

#include "AsioEventProcessor.h"

#include "common/Log.h"

#include <memory>

namespace Common {

//===============================================================================

namespace {
	std::shared_ptr<spdlog::logger> s_logger;
} // anon namespace

AsioEventProcessor::AsioEventProcessor() : m_ios(std::make_unique<asio::io_service>())
{
	m_work.reset(new asio::io_service::work(*m_ios));
	m_ios->post([this]()
		{
			m_isReady = true;
		});
}

AsioEventProcessor::~AsioEventProcessor()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

asio::io_service& AsioEventProcessor::GetIoService()
{
	return *m_ios;
}

void AsioEventProcessor::Post(const std::function<void()>& cb)
{
	m_ios->post(cb);
}

void AsioEventProcessor::Run()
{
	m_thread = std::thread(([this]() { DoRun(); }));
}

void AsioEventProcessor::DoRun()
{
	std::error_code ec;
	m_ios->run(ec);

	if (ec)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "ASIO error occurred while running a task. ec={}",
			ec.value());
	}
}

//===============================================================================

} // namespace common
