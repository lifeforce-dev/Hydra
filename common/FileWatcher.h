//---------------------------------------------------------------
//
// FileWatcher.h
//

#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

namespace Common {

//===============================================================================

enum class FileStatus
{
	CREATED = 0,
	MODIFIED,
	ERASED
};

using FileEventCallback = std::function<void(const std::string&, FileStatus)>;
class FileWatcher {
public:
	FileWatcher(const std::string& relativePath, std::chrono::duration<int, std::milli> delay);
	~FileWatcher();

	// NOTE: Might change this implementation to use OS events, but this implementation was fun.
	// https://docs.microsoft.com/en-us/windows/win32/fileio/obtaining-directory-change-notifications
	void Run(const FileEventCallback& action);
	void Stop();

private:
	void DoRun(const FileEventCallback& cb);

	// Time to wait between checking.
	std::chrono::duration<int, std::milli> m_delay;

	// Root directory to watch.
	std::string m_rootWatchDir;

	// Cache of paths we know about mapped to the time they were last modified.
	std::unordered_map<std::string, std::filesystem::file_time_type> m_paths = {};

	// Thread to run this on.
	std::thread m_thread;

	// Run time sentinel.
	bool m_isRunning = true;
};

//===============================================================================

} // namespace Common
