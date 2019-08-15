//---------------------------------------------------------------
//
// Log.h
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace std::experimental::filesystem::v1;
namespace Logger {

//==============================================================================

// TODO:
	// init logging by registering log sinks
	// functions to get loggers from sinks
	// all loggers should be be async
class AsyncLogger {
public:
	AsyncLogger()
		: m_consoleSink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>())
		, m_fileSink(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true))
	{
		m_consoleSink->set_level(spdlog::level::trace);
		m_consoleSink->set_pattern("[console_sink] [%^%l%$] %v");
		m_fileSink->set_level(spdlog::level::trace);
		m_sinks.push_back(m_consoleSink);
		m_sinks.push_back(m_fileSink);
		m_logger = std::make_shared<spdlog::async_logger>("logger",
			m_sinks.begin(), m_sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	}
	~AsyncLogger() {}

	const std::shared_ptr<spdlog::async_logger>& GetLogger() { return m_logger; }

private:
	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> m_consoleSink;
	std::shared_ptr< spdlog::sinks::basic_file_sink_mt> m_fileSink;
	std::vector<spdlog::sink_ptr> m_sinks;

	std::shared_ptr<spdlog::async_logger> m_logger;
};

static bool loggerInitialized = false;
static spdlog::logger& Logger()
{
	spdlog::init_thread_pool(32768, 1);
	static std::string path = "logs/log.txt";
	if (!is_regular_file(path))
	{
		// create log file if it doesn't exist
		create_directory("logs");
		std::ofstream("logs/log.txt");
	}

	static AsyncLogger asyncLogger;
	return *asyncLogger.GetLogger();
}
//==============================================================================

} // namespace Logger
