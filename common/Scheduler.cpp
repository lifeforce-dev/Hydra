//---------------------------------------------------------------
//
// Scheduler.cpp
//

#include "common/Scheduler.h"
#include "common/Log.h"

namespace Common {

//===============================================================================

namespace
{
	uint64_t s_idPool = -1;

	std::shared_ptr<spdlog::logger> s_logger;
}

// todo add context (some enum that says what this thing is for).
Task::Task(const std::shared_ptr<void>& ownerHandle, const TaskCallback& callback)
	: m_ownerHandle(ownerHandle)
	, m_callback(callback)
	, m_id(++s_idPool)
{
	Initialize();
}

Task::Task(const std::shared_ptr<void>& ownerHandle, const TaskCallback& callback,
	const Options& options)
	: m_ownerHandle(ownerHandle)
	, m_callback(callback)
	, m_intervalMs(options.intervalMs)
	, m_taskCostMs(options.taskCostMs)
	, m_hasDelayedTick(options.hasDelayedTick)
	, m_id(++s_idPool)
{
	Initialize();
}

Task::Task(Task&& rhs)
	: m_ownerHandle(std::move(rhs.m_ownerHandle))
	, m_intervalMs(std::move(rhs.m_intervalMs))
	, m_creationTimeStamp(std::move(rhs.m_creationTimeStamp))
	, m_nextTimeToExecute(std::move(rhs.m_nextTimeToExecute))
	, m_lastTimeCalled(std::move(rhs.m_lastTimeCalled))
	, m_hasDelayedTick(std::move(rhs.m_hasDelayedTick))
	, m_isCanceled(std::move(rhs.m_isCanceled))
	, m_taskCostMs(std::move(rhs.m_taskCostMs))
	, m_id(std::move(rhs.m_id))
	, m_callback(std::move(m_callback))
{
	Initialize();
}

Common::Task& Task::operator=(Task&& rhs)
{
	m_ownerHandle = std::move(rhs.m_ownerHandle);
	m_intervalMs = std::move(rhs.m_intervalMs);
	m_creationTimeStamp = std::move(rhs.m_creationTimeStamp);
	m_nextTimeToExecute = std::move(rhs.m_nextTimeToExecute);
	m_lastTimeCalled = std::move(rhs.m_lastTimeCalled);
	m_hasDelayedTick = std::move(rhs.m_hasDelayedTick);
	m_isCanceled = std::move(rhs.m_isCanceled);
	m_taskCostMs = std::move(rhs.m_taskCostMs);
	m_id = std::move(rhs.m_id);
	m_callback = std::move(rhs.m_callback);

	return *this;
}

void Task::Execute()
{
	auto sp = m_ownerHandle.lock();
	if (!sp)
	{
		return;
	}

	m_lastTimeCalled = std::chrono::steady_clock::now();

	//TaskContext context;
	//context.id = m_id;
	//context.status = TaskCompletionStatus::Complete;
	//m_callback(context);
}

const std::chrono::milliseconds& Task::GetIntervalMs() const
{
	return m_intervalMs;
}

const std::chrono::steady_clock::time_point& Task::GetCreationTimeStamp() const
{
	return m_creationTimeStamp;
}

uint64_t Task::GetId() const
{
	return m_id;
}

const std::chrono::milliseconds& Task::GetTaskCostMs() const
{
	return m_taskCostMs;
}

bool Task::GetHasDelayedTick() const
{
	return m_hasDelayedTick;
}

void Task::SetHasDelayedTick(bool hasDelayedTick)
{
	m_hasDelayedTick = hasDelayedTick;
}

const std::chrono::steady_clock::time_point& Task::GetNextTimeToExecute() const
{
	return m_nextTimeToExecute;
}

void Task::SetNextTimeToExecute(const std::chrono::steady_clock::time_point& nextCallTime)
{
	m_nextTimeToExecute = nextCallTime;
}

void Task::SetLastCallbackTime(const std::chrono::steady_clock::time_point& lastCallTime)
{
	m_lastTimeCalled = lastCallTime;
}

const std::chrono::steady_clock::time_point& Task::GetLastTimeCalled() const
{
	return m_lastTimeCalled;
}

bool Task::GetIsCanceled() const
{
	return m_isCanceled;
}

void Task::SetIsCanceled(bool isCanceled)
{
	m_isCanceled = isCanceled;
}

void Task::Initialize()
{
	REGISTER_LOGGER("Task");
	s_logger = Log::Logger("Task");

	m_creationTimeStamp = std::chrono::steady_clock::now();
	//SPDLOG_LOGGER_TRACE(s_logger, "Task created")
}

//===============================================================================


} // namespace Common
