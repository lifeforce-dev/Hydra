//---------------------------------------------------------------
//
// Timer.h
//

#pragma once

#include <chrono>

using SteadyClock = std::chrono::steady_clock;

namespace Common {

//===============================================================================

class Timer {

public:
	Timer();
	~Timer();

	// Impossible to be stopped and paused at the same time.
	void Pause();

	// Will switch off paused state and continue running.
	void Resume();

	// Begins the timer. Does nothing if already running.
	void Start();

	// Clears all state and calls Start. Can be executed from any state.
	void Restart();

	// Stops the timer. Timer is able to be resumed by calling Start.
	// Unlike pausing, resuming from stopped will clear all state.
	void Stop();

	// Clears all state, effectively stopping the timer. Can be executed from any state.
	void Clear();

	// Returns the amount of time this timer has been actually running (not including paused).
	std::chrono::milliseconds GetElapsedMs() const;
	std::chrono::microseconds GetElapsedUs() const;

	// Returns the amount of time elapsed for current pause session only.
	std::chrono::milliseconds GetElapsedPausedMs() const;
	std::chrono::microseconds GetElapsedPausedUs() const;

	// Returns the cumulative amount of time the timer has been paused.
	std::chrono::milliseconds GetTotalElapsedPausedMs() const;
	std::chrono::microseconds GetTotalElapsedPausedUs() const;

	// Returns the total amount of time that this timer has been active (including paused).
	std::chrono::milliseconds GetTotalElapsedMs() const;
	std::chrono::microseconds GetTotalElapsedUs() const;

	// True if paused. Paused implies running.
	bool IsPaused() const;

	// True if running. Not running implies not paused.
	bool IsRunning() const;

private:
	std::chrono::microseconds DoGetElapsed() const;

private:
	bool m_isRunning = false;
	bool m_isPaused = false;

	SteadyClock::time_point m_startTimeStamp;
	SteadyClock::time_point m_endTimeStamp;
	SteadyClock::time_point m_pauseTimeStamp;

	std::chrono::microseconds m_totalPauseDuration = std::chrono::microseconds::zero();
};

//===============================================================================

} // namespace Common
