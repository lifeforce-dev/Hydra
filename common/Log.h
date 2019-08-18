//---------------------------------------------------------------
//
// Log.h
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace std::experimental::filesystem::v1;
namespace Log {

//==============================================================================

static const std::string& GetLogFile()
{
	static std::string s_logFile = "../logs/log.txt";
	return s_logFile;
}

class LoggerManager
{
public:
	LoggerManager();
	~LoggerManager();

void CreateLogger(const std::string& loggerName);

private:
	void Init();
	std::shared_ptr<spdlog::sinks::basic_file_sink_mt> m_sharedSink;
};

void CreateAndRegisterLogger(const std::string& name);

static std::shared_ptr<spdlog::logger> Logger(const std::string& name)
{
	return spdlog::get(name);
}

#define REGISTER_LOGGER(name)            \
{                                        \
	Log::CreateAndRegisterLogger(name);  \
}

//==============================================================================

} // namespace Logger

