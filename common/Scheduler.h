//---------------------------------------------------------------
//
// Scheduler.h
//

#pragma once

#include "common/Task.h"
#include "common/Timer.h"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <stdint.h>
#include <queue>
#include <vector>
#include <optional>

namespace Common {

//===============================================================================

enum class TaskCompletionStatus : uint8_t
{
	Complete,
	Canceled
};

using TaskCallback = std::function<void()>;

class Task
{
public:
	struct Options;

public:
	Task(const std::shared_ptr<void>& ownerHandle, const TaskCallback& callback);
	Task(const std::shared_ptr<void>& ownerHandle, const TaskCallback& callback,
		const Options& options);

	Task(Task&& rhs);
	Task& operator=(Task&& rhs);

	Task(const Task& rhs) = delete;
	Task& operator=(const Task& rhs) = delete;

	// Executes the task callback.
	void Execute();

	// Read only fields. These cannot be set after creation.

	// Returns the interval at which the callback will be called.
	const std::chrono::milliseconds& GetIntervalMs() const;

	// Returns the time cost of the task.
	const std::chrono::milliseconds& GetTaskCostMs() const;

	// Returns the timestamp of when this Task was created.
	const std::chrono::steady_clock::time_point& GetCreationTimeStamp() const;

	// Returns the task id, guaranteed to be unique.
	uint64_t GetId() const;

	//-------------------------------------------------------------------------------

	// Read/write fields.

	// Returns whether we should wait an interval before executing.
	bool GetHasDelayedTick() const;

	// Sets whether to delay execution for an interval. This is set to false internally after first execution.
	void SetHasDelayedTick(bool hasDelayedTick);

	// Returns the next expected time the callback should be called.
	const std::chrono::steady_clock::time_point& GetNextTimeToExecute() const;

	// Sets the next expected time the callback should be called.
	void SetNextTimeToExecute(const std::chrono::steady_clock::time_point& nextCallTime);

	// Timestamp the time at which the task callback was called.
	void SetLastCallbackTime(const std::chrono::steady_clock::time_point& lastCallTime);

	// Returns the timestamp of the last time the callback was called.
	const std::chrono::steady_clock::time_point& GetLastTimeCalled() const;

	// Returns whether this task has been canceled.
	bool GetIsCanceled() const;

	// Mark this task for cancellation. Its callback will not be executed.
	void SetIsCanceled(bool isCanceled);

	struct Options
	{
		std::chrono::milliseconds intervalMs = std::chrono::milliseconds::zero();
		std::chrono::milliseconds taskCostMs = std::chrono::milliseconds::zero();
		bool hasDelayedTick = false;
	};

private:
	void Initialize();

private:
	// The owner of the callback we now have a reference to.
	// This is used to make sure the owner is still alive when we execute.
	std::weak_ptr<void> m_ownerHandle;

	// Not required, but if set will cause callback to continually be called at this interval.
	std::chrono::milliseconds m_intervalMs = std::chrono::milliseconds::zero();

	// Set when this Task is created. Used for logging.
	std::chrono::steady_clock::time_point m_creationTimeStamp;

	// Indicates when the callback should be called.
	std::chrono::steady_clock::time_point m_nextTimeToExecute;

	// Set at the point of calling the callback.
	std::chrono::steady_clock::time_point m_lastTimeCalled;

	// Indicates whether we should wait a full interval before executing the first time.
	bool m_hasDelayedTick = false;

	// Callback will not be called if this is set before its time to execute.
	bool m_isCanceled = false;

	// If not set, the task is instant. Otherwise, the time cost is added to next call time.
	// Could reflect a cast time or something similar.
	std::chrono::milliseconds m_taskCostMs = std::chrono::milliseconds::zero();

	// Unique identifier for this callback.
	uint64_t m_id = 0;

	// Callback we will execute when the time comes.
	TaskCallback m_callback;
};

template<typename T>
class SchedulerBase
{
	using timer_clock = T;
	using time_point = typename timer_clock::time_point;
	struct TimedCallbackComparator
	{
		bool operator()(const Task& lhs, const Task& rhs) const
		{
			return lhs.GetNextTimeToExecute() > rhs.GetNextTimeToExecute();
		}
	};

	using TimedCbPriorityQueue = std::vector<Task>;
public:
	SchedulerBase()
	{

	}

	SchedulerBase(SchedulerBase&& rhs)
		: m_currentTime(std::move(rhs.m_currentTime))
		, m_isRunning(std::move(rhs.m_isRunning))
		, m_sortedTasks(std::move(rhs.m_sortedTasks))
	{
	}

	//SchedulerBase(const Scheduler& rhs) = delete;
	~SchedulerBase() {}

	// Processes the head of the queue each call.
	void Process()
	{
		using namespace std::chrono;
		if (m_sortedTasks.empty())
		{
			return;
		}

		Task cb = std::move(m_sortedTasks.front());
		m_sortedTasks.pop();

		m_currentTime = now();

		if (HandleDelayedTick(cb))
		{
			return;
		}

		steady_clock::time_point expectedCallTime = cb.GetNextTimeToExecute();

		// impl
		// is it time to run it yet?
		// check the work cost
		// if the work cost is 0
			// run the event
		// else 
			// add work cost to timer and place back into queue
		// This will allow time to keep being added to the event (knock back, slow down, speed up etc)
		if (m_currentTime >= expectedCallTime)
		{
			cb.Execute();

			// If we overshot our expected call time, fix the next call time with the difference.
			// This is to prevent distance between call times to increase infinitely.
			if (cb.GetIntervalMs() != milliseconds::zero())
			{
				milliseconds overshoot = duration_cast<milliseconds>(m_currentTime - expectedCallTime);
				milliseconds offset = cb.GetIntervalMs() - overshoot;
				cb.SetNextTimeToExecute(m_currentTime + offset);
			}
		}
	}

	// Will create a new task with the params given and add it to the queue.
	uint64_t Post(const std::shared_ptr<void>& ownerHandle,
		const std::optional<Task::Options>& options,
		const Task& callback)
	{
		//Task cb(ownerHandle, callback, options.value_or(TaskOptions{}));
		//uint64_t cbId = cb.GetId();
		//AddToPriorityQueue(std::move(cb));

		return 0;
	}

	// Returns true if Scheduler should wait a full interval to process task.
	bool HandleDelayedTick(Task& cb)
	{
		if (cb.GetHasDelayedTick())
		{
			cb.SetNextTimeToExecute(m_currentTime + cb.GetIntervalMs());
			cb.SetHasDelayedTick(false);
			m_sortedTasks.push(std::move(cb));

			return true;
		}

		return false;
	}

private:
	static time_point now() { return timer_clock::now(); }

private:
	std::chrono::steady_clock::time_point m_currentTime = now();
	std::priority_queue<Task> m_sortedTasks;

	bool m_isRunning = true;
};

class Scheduler : public SchedulerBase<std::chrono::steady_clock> {};
class MockScheduler : public SchedulerBase<MockClock> {};

//===============================================================================

}

