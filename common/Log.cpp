//---------------------------------------------------------------
//
// Log.cpp
//

#include "common/Log.h"

namespace Log {

LoggerManager::LoggerManager()
	: m_sharedSink(std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetLogFile(), true))
{
	Init();
}

LoggerManager::~LoggerManager()
{

}

void LoggerManager::CreateLogger(const std::string& loggerName)
{
	// Initialize the log sinks.
	std::vector<spdlog::sink_ptr> sinks;

	using namespace spdlog::sinks;

	auto msvcSink = std::make_shared<msvc_sink_mt>();
	msvcSink->set_level(spdlog::level::trace);
	msvcSink->set_pattern("%^%l%$ %Y-%m-%d %T.%f [%n] %v (%@)");
	sinks.push_back(msvcSink);
	sinks.push_back(m_sharedSink);

	auto logger = std::make_shared<spdlog::async_logger>(loggerName,
		std::begin(sinks), std::end(sinks), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

	spdlog::register_logger(logger);
}

void LoggerManager::Init()
{
	spdlog::init_thread_pool(32768, 1);
	spdlog::flush_every(std::chrono::seconds(1));

	m_sharedSink->set_level(spdlog::level::trace);
}

void CreateAndRegisterLogger(const std::string& name)
{
	static LoggerManager loggerManager;
	loggerManager.CreateLogger(name);
}

} // namespace Logger
