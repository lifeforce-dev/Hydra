//---------------------------------------------------------------
//
// Log.h
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Logger {

//==============================================================================

static bool loggerInitialized = false;
static spdlog::logger& Logger()
{
	static auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_level(spdlog::level::warn);
	consoleSink->set_pattern("[console_sink] [%^%l%$] %v");

	static auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
	fileSink->set_level(spdlog::level::trace);

	static spdlog::logger logger("multiSink", {consoleSink, fileSink});
	return logger;
}
//==============================================================================

} // namespace Logger
