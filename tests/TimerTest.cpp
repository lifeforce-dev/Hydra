//---------------------------------------------------------------
//
// TimerTest.cpp
//

#include "TimerTest.h"

#include "Catch2/catch.hpp"

#include <chrono>
#include <thread>

namespace Test {

//===============================================================================

namespace {
	// Allow timers that are a few ms off to pass as its likely due to debug mode.
	uint32_t acceptableMarginOfErrorMs = 5;

	// Timing is hard in debug mode. If we're within a few ms or so, I think that's okay.
	auto isRoughlyCorrect = [](long long value, uint32_t target) -> bool
	{
		bool isInRange = value <= target + static_cast<long long>(acceptableMarginOfErrorMs)
			&& value >= target - static_cast<long long>(acceptableMarginOfErrorMs);
		return value == target || isInRange;
	};
}

SCENARIO("Calling Start multiple times still produces correct output.", "[Timer]")
{
	using namespace std::chrono;
	GIVEN("A started timer and a loop that checkes elapsed time")
	{
		Common::Timer t;
		t.Start();
		SteadyClock::time_point start = SteadyClock::now();

		WHEN("Calling Start() on timer continually for 5 seconds completes")
		{
			while (duration_cast<milliseconds>(SteadyClock::now() - start).count() != 50)
			{
				t.Start();
			}

			REQUIRE(t.GetElapsedMs().count() == 50);
		}
	}
}

SCENARIO("Pausing and Unpausing while Timer is running produces correct output.", "[Timer]")
{
	using namespace std::chrono;
	uint32_t timeToSleepMs = 50;
	bool intermediateChecksPass = true;

	GIVEN("A running timer")
	{
		Common::Timer timer;
		WHEN("Timer is paused an unpaused every second for 5 seconds")
		{
			timer.Start();

			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			timer.Pause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= timer.IsPaused();

			timer.Unpause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();

			timer.Pause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 100);
			intermediateChecksPass &= timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			timer.Unpause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 150);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimeMs().count(), 250);
			REQUIRE(intermediateChecksPass);
		}
	}
}

SCENARIO("Starting and stopping the timer produces correct results.", "[Timer]")
{
	using namespace std::chrono;

	uint32_t timeToSleepMs = 50;
	bool intermediateChecksPass = true;
	GIVEN("A newly created timer.")
	{
		Common::Timer timer;

		WHEN("Time is started, stopped, and paused repeatedly.")
		{
			// Ensure state is reset properly after starting again from a stop.
			timer.Start();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= timer.IsRunning();

			timer.Pause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			auto i = timer.GetElapsedMs().count();
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= timer.IsPaused();

			timer.Unpause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 100);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= !timer.IsPaused();

			timer.Stop();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 100);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 50);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= !timer.IsRunning();

			timer.Start();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			timer.Stop();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= !timer.IsRunning();

			// Ensure timer will not pause or unpause while stopped.
			timer.Pause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= !timer.IsPaused();

			timer.Unpause();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));
			intermediateChecksPass &= !timer.IsRunning();

			REQUIRE(intermediateChecksPass);
		}
	}
}

SCENARIO("Restarting timer properly restarts it.", "[Timer]")
{
	using namespace std::chrono;

	uint32_t timeToSleepMs = 50;
	bool intermediateChecksPass = true;

	GIVEN("A timer with a bunch of established state")
	{
		Common::Timer timer;
		timer.Start();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		timer.Pause();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		timer.Unpause();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		timer.Pause();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		intermediateChecksPass &= timer.IsPaused();
		intermediateChecksPass &= timer.IsRunning();
		intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 100);
		intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 50);
		intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 100);

		WHEN("Timer is restarted from a paused state")
		{
			timer.Restart();
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			std::this_thread::sleep_for(milliseconds(timeToSleepMs));

			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			REQUIRE(intermediateChecksPass);
		}
		AND_WHEN("Timer is restarted from a running state")
		{
			timer.Unpause();
			timer.Restart();
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			std::this_thread::sleep_for(milliseconds(timeToSleepMs));

			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			REQUIRE(intermediateChecksPass);
		}
		AND_WHEN("Timer is restarted from a stopped state")
		{
			timer.Stop();
			timer.Restart();
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			std::this_thread::sleep_for(milliseconds(timeToSleepMs));

			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();

			REQUIRE(intermediateChecksPass);
		}
	}
}

SCENARIO("Clearing a timer", "[Timer]")
{
	using namespace std::chrono;

	uint32_t timeToSleepMs = 50;
	bool intermediateChecksPass = true;
	GIVEN("A timer with a bunch of established state")
	{
		Common::Timer timer;
		timer.Start();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		timer.Pause();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		timer.Unpause();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		timer.Pause();
		std::this_thread::sleep_for(milliseconds(timeToSleepMs));

		intermediateChecksPass &= timer.IsPaused();
		intermediateChecksPass &= timer.IsRunning();
		intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 100);
		intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 50);
		intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 100);

		WHEN("Clearing the timer")
		{
			timer.Clear();
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= !timer.IsRunning();

			std::this_thread::sleep_for(milliseconds(timeToSleepMs));

			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= !timer.IsRunning();
		}
		WHEN("Starting the timer after the clear")
		{
			timer.Clear();
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= !timer.IsRunning();

			timer.Start();
			std::this_thread::sleep_for(milliseconds(timeToSleepMs));

			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedMs().count(), 50);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= isRoughlyCorrect(timer.GetTotalElapsedTimePausedMs().count(), 0);
			intermediateChecksPass &= !timer.IsPaused();
			intermediateChecksPass &= timer.IsRunning();
		}
	}
}

//===============================================================================

} // namespace Test
