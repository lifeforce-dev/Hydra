//---------------------------------------------------------------
//
// Log.h
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

namespace Log {

//==============================================================================

static const std::string& GetLogFile()
{
	static std::string s_logFile = "../logs/log.log";
	return s_logFile;
}

static const std::string& GetTestsLogFile()
{
	static std::string s_testsLogFile = "../logs/tests/tests.log";
	return s_testsLogFile;
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

