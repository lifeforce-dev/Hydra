//---------------------------------------------------------------
//
// Timer.h
//

#pragma once

#include <chrono>

namespace Common {

//===============================================================================

// https://en.cppreference.com/w/cpp/named_req/Clock
class MockClock {
public:
	using time_point = std::chrono::steady_clock::time_point;
	using rep = std::chrono::steady_clock::rep;
	using period = std::chrono::steady_clock::period;
	using duration = std::chrono::steady_clock::duration;

	inline static constexpr bool is_steady = true;
	inline static time_point now_time = {};

	static time_point now() { return now_time; }
};

template<typename T>
class TimerBase {

	using timer_clock = T;
	using time_point = typename timer_clock::time_point;
public:
	TimerBase()
	{
	}

	virtual ~TimerBase()
	{
		Stop();
	}

	// Impossible to be stopped and paused at the same time.
	void Pause()
	{
		if (!m_isRunning)
		{
			return;
		}

		m_pauseTimeStamp = now();
		m_isPaused = true;
	}

	// Will switch off paused state and continue running.
	void Resume()
	{
		if (!m_isPaused)
		{
			return;
		}

		using namespace std::chrono;

		m_isPaused = false;
		m_totalPauseDuration += duration_cast<microseconds>(now() - m_pauseTimeStamp);
		m_pauseTimeStamp = time_point{};
	}

	// Begins the timer. Does nothing if already running.
	void Start()
	{
		if (m_isRunning)
		{
			return;
		}

		Clear();
		m_isRunning = true;
		m_startTimeStamp = now();
		m_endTimeStamp = time_point{};
	}

	// Clears all state and calls Start. Can be executed from any state.
	void Restart()
	{
		Clear();
		Start();
	}

	// Stops the timer. Timer is able to be resumed by calling Start.
	// Unlike pausing, resuming from stopped will clear all state.
	void Stop()
	{
		if (!m_isRunning)
		{
			return;
		}

		m_isRunning = false;
		m_isPaused = false;
		m_endTimeStamp = now();
	}

	// Clears all state, effectively stopping the timer. Can be executed from any state.
	void Clear()
	{
		using namespace std::chrono;

		m_isRunning = false;
		m_isPaused = false;
		m_startTimeStamp = time_point{};
		m_endTimeStamp = time_point{};
		m_pauseTimeStamp = time_point{};
		m_totalPauseDuration = microseconds::zero();
	}

	// Returns the amount of time this timer has been actually running (not including paused).
	std::chrono::milliseconds GetElapsedMs() const
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(GetElapsedUs());
	}

	std::chrono::microseconds GetElapsedUs() const
	{
		// If we're currently paused we need to consider how long we've been paused for.
		if (m_isPaused)
		{
			return DoGetElapsed() - (m_totalPauseDuration + GetElapsedPausedUs());
		}

		return DoGetElapsed() - m_totalPauseDuration;
	}

	// Returns the amount of time elapsed for current pause session only.
	std::chrono::milliseconds GetElapsedPausedMs() const
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(GetElapsedPausedUs());
	}

	std::chrono::microseconds GetElapsedPausedUs() const
	{
		using namespace std::chrono;

		// Running time stops keeping track once paused. Return only up to paused point.
		if (m_isPaused)
		{
			return duration_cast<microseconds>(now() - m_pauseTimeStamp);
		}

		// We're not paused, so this query doesn't make sense. Return default value (0).
		return microseconds::zero();
	}

	// Returns the cumulative amount of time the timer has been paused.
	std::chrono::milliseconds GetTotalElapsedPausedMs() const
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(GetTotalElapsedPausedUs());
	}

	std::chrono::microseconds GetTotalElapsedPausedUs() const
	{
		// If we're currently paused, the clock is still running for this pause session.
		if (m_isPaused)
		{
			return m_totalPauseDuration + GetElapsedPausedUs();
		}

		return m_totalPauseDuration;
	}

	// Returns the total amount of time that this timer has been active (including paused).

	std::chrono::milliseconds GetTotalElapsedMs() const
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(GetTotalElapsedUs());
	}

	std::chrono::microseconds GetTotalElapsedUs() const
	{
		return DoGetElapsed();
	}

	// True if paused. Paused implies running.
	bool IsPaused() const
	{
		return m_isPaused;
	}

	// True if running. Not running implies not paused.
	bool IsRunning() const
	{
		return m_isRunning;
	}

private:
	std::chrono::microseconds DoGetElapsed() const
	{
		using namespace std::chrono;

		if (!m_isRunning)
		{
			// If we stopped, then we only care about the time when we were running until we stopped.
			return duration_cast<microseconds>(m_endTimeStamp - m_startTimeStamp);
		}

		// We're currently running and just want an update on how much time has elapsed.
		return duration_cast<microseconds>(now() - m_startTimeStamp);
	}

private:
	static time_point now() { return timer_clock::now(); }
	bool m_isRunning = false;
	bool m_isPaused = false;

	time_point m_startTimeStamp;
	time_point m_endTimeStamp;
	time_point m_pauseTimeStamp;

	std::chrono::microseconds m_totalPauseDuration = std::chrono::microseconds::zero();
};

class Timer : public TimerBase<std::chrono::steady_clock> {};
class MockTimer : public TimerBase<MockClock> {};

//===============================================================================

} // namespace Common
