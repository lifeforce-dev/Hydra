//---------------------------------------------------------------
//
// FileWatcher.cpp
//
// Inspired heavily by https://solarianprogrammer.com/2019/01/13/cpp-17-filesystem-write-file-watcher-monitor/
//

#include "FileWatcher.h"
#include <assert.h>

namespace Common {

//===============================================================================

namespace {

const std::thread::id s_mainThreadId = std::this_thread::get_id();

} // anon namespace

FileWatcher::FileWatcher(const std::string& relativePath,
	std::chrono::duration<int, std::milli> delay)
	: m_delay(delay)
	, m_rootWatchDir(relativePath)
{
	namespace fs = std::filesystem;

	// Initialize file list.
	for (auto& file : fs::recursive_directory_iterator(m_rootWatchDir))
	{
		auto currentFileLastWriteTime = fs::last_write_time(file);
		m_paths[file.path().string()] = currentFileLastWriteTime;
	}
}

FileWatcher::~FileWatcher()
{
	Stop();
}

void FileWatcher::Run(const FileEventCallback& cb)
{
	m_thread = std::thread([this, cb]()
	{
		DoRun(cb);
	});
}

void FileWatcher::Stop()
{
	m_isRunning = false;
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void FileWatcher::DoRun(const FileEventCallback& cb)
{
	// Ensure that we never run this on the main thread.
	assert(std::this_thread::get_id() != s_mainThreadId);

	while (m_isRunning)
	{
		// Yup its a sleep, don't care.
		std::this_thread::sleep_for(m_delay);

		namespace fs = std::filesystem;

		// Clean up any files that might have been deleted.
		auto it = m_paths.begin();
		while (it != m_paths.end())
		{
			if (!fs::exists(it->first))
			{
				cb(it->first, FileStatus::ERASED);
				it = m_paths.erase(it);
			}
			else
			{
				it++;
			}
		}

		for (auto& file : fs::recursive_directory_iterator(m_rootWatchDir))
		{
			auto currentFileLastWriteTime = fs::last_write_time(file);
			const auto& key = file.path().string();

			// Update file list with newly found files.
			if (m_paths.find(key) == m_paths.end())
			{
				m_paths[key] = currentFileLastWriteTime;
				cb(key, FileStatus::CREATED);

				// Brand new files can't also count as modified or anything else.
				continue;
			}

			// Update modified file
			if (m_paths[key] != currentFileLastWriteTime)
			{
				m_paths[key] = currentFileLastWriteTime;
				cb(key, FileStatus::MODIFIED);
			}
		}
	}
}

//===============================================================================

} // namespace Common
