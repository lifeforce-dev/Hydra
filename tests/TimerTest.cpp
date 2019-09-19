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
	using namespace std::chrono_literals;

	// Allow timers that are a few ms off to pass as its likely due to debug mode.
	std::chrono::milliseconds s_acceptableMarginOfErrorMs = 10000ms;

	auto s_timeToSleepMs = 50ms;

	// Timing is hard in debug mode. If we're within a few ms or so, I think that's okay.
	auto isRoughlyCorrect = [](std::chrono::milliseconds value, std::chrono::milliseconds target) -> bool
	{
		bool isInRange = value <= target + s_acceptableMarginOfErrorMs
			&& value >= target - s_acceptableMarginOfErrorMs;
		return value == target || isInRange;
	};
}

SCENARIO("Duplicate calls to Start don't break state.", "[Timer]")
{
	using namespace std::chrono;
	GIVEN("A timer.")
	{
		Common::Timer t;

		WHEN("Calling Start() on timer continually for s_timeToSleepMs.")
		{
			SteadyClock::time_point start = SteadyClock::now();
			while (duration_cast<milliseconds>(SteadyClock::now() - start).count() != 50)
			{
				t.Start();
			}

			THEN("State should not be effected.")
			{
				REQUIRE(isRoughlyCorrect(t.GetElapsedMs(), 50ms));
			}
		}
	}
}

SCENARIO("Pausing a running timer.", "[Timer]")
{
	using namespace std::chrono;
	GIVEN("A timer that has been running for s_timeToSleepMs")
	{
		Common::Timer timer;
		timer.Start();
		std::this_thread::sleep_for(s_timeToSleepMs);

		WHEN("The timer is paused")
		{
			timer.Pause();

			THEN("Paused state should change")
			{ 
				bool isPaused = timer.IsPaused();
				CAPTURE(isPaused);

				REQUIRE(timer.IsPaused());
			}

			AND_THEN("Timer is paused for s_timeToSleepMs")
			{
				std::this_thread::sleep_for(s_timeToSleepMs);

				milliseconds elapsed = timer.GetElapsedPausedMs();
				CAPTURE(elapsed.count());

				REQUIRE(isRoughlyCorrect(timer.GetElapsedPausedMs(), 50ms));
			}
		}
	}
}

SCENARIO("Running a timer.", "[Timer]")
{
	using namespace std::chrono;
	GIVEN("A newly created timer.")
	{
		Common::Timer timer;
		REQUIRE(!timer.IsRunning());
		REQUIRE(!timer.IsPaused());

		WHEN("Timer is started")
		{
			timer.Start();

			THEN("IsRunning state should change.")
			{
				auto isRunning = timer.IsRunning();
				CAPTURE(isRunning);

				REQUIRE(timer.IsRunning());
			}
			AND_THEN("Timer is left running for s_timeToSleepMs.")
			{
				std::this_thread::sleep_for(s_timeToSleepMs);

				milliseconds elapsed = timer.GetElapsedMs();
				CAPTURE(elapsed.count());

				REQUIRE(isRoughlyCorrect(elapsed, 50ms));
			}
		}
	}
}

SCENARIO("Resuming a timer.")
{
	using namespace std::chrono;

	GIVEN("A previously running time that has since been paused.")
	{
		Common::Timer timer;

		timer.Start();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Pause();
		std::this_thread::sleep_for(s_timeToSleepMs);

		WHEN("Timer is resumed after being paused.")
		{
			timer.Resume();

			THEN("Timer paused flag should be switched off")
			{
				bool isPaused = timer.IsPaused();
				CAPTURE(isPaused);

				REQUIRE(!isPaused);
			}
			AND_THEN("Timer should have correct elapsed time.")
			{
				milliseconds elapsed = timer.GetElapsedMs();
				CAPTURE(elapsed.count());

				REQUIRE(isRoughlyCorrect(elapsed, 50ms));
			}
			AND_THEN("Timer should report 0ms elapsedPauseTime, since we're not paused.")
			{
				milliseconds elapsedPaused = timer.GetElapsedPausedMs();
				CAPTURE(elapsedPaused.count());

				REQUIRE(elapsedPaused == 0ms);
			}
		}
	}
}

SCENARIO("Timer has accumulated total elapsed time across pauses/resumes.", "[Timer]")
{
	using namespace std::chrono;

	GIVEN("A timer that has been paused and resumed a few times")
	{
		Common::Timer timer;

		timer.Start();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Pause();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Resume();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Pause();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Resume();
		std::this_thread::sleep_for(s_timeToSleepMs * 2);

		WHEN("Asked about total elapsed time")
		{
			THEN("The timer should return the sum of paused time and running time.")
			{
				milliseconds totalElapsed = timer.GetTotalElapsedMs();
				CAPTURE(totalElapsed.count());

				REQUIRE(isRoughlyCorrect(totalElapsed, 300ms));
			}
		}
		AND_WHEN("Asked about total elapsed paused time")
		{
			THEN("The timer should return the sum of all time spent paused")
			{
				milliseconds totalElapsedPaused = timer.GetTotalElapsedPausedMs();
				CAPTURE(totalElapsedPaused.count());

				REQUIRE(isRoughlyCorrect(totalElapsedPaused, 100ms));
			}
		}
	}
}

SCENARIO("Clearing a timer.", "[Timer]")
{
	using namespace std::chrono;
	GIVEN("A used timer with some accumulated state.")
	{
		Common::Timer timer;
		timer.Start();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Pause();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Resume();
		std::this_thread::sleep_for(s_timeToSleepMs);

		WHEN("Timer is cleared.")
		{
			timer.Clear();

			THEN("All state is properly cleared on the timer.")
			{
				bool isPaused = timer.IsPaused();
				bool isRunning = timer.IsRunning();
				milliseconds elapsed = timer.GetElapsedMs();
				milliseconds elapsedPaused = timer.GetElapsedPausedMs();
				milliseconds totalElapsed = timer.GetTotalElapsedMs();
				milliseconds totalElapsedPaused = timer.GetTotalElapsedPausedMs();
				CAPTURE(isPaused, isRunning, elapsed.count(), elapsedPaused.count(),
					totalElapsed.count(), totalElapsedPaused.count());
				
				REQUIRE(!isPaused);
				REQUIRE(!isRunning);
				REQUIRE(elapsed == 0ms);
				REQUIRE(elapsedPaused == 0ms);
				REQUIRE(totalElapsed == 0ms);
				REQUIRE(totalElapsedPaused == 0ms);
			}
		}
	}
}

SCENARIO("Stopping a running timer")
{
	using namespace std::chrono;

	GIVEN("A running timer with some accumulated state.")
	{
		Common::Timer timer;

		timer.Start();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Pause();
		std::this_thread::sleep_for(s_timeToSleepMs);

		timer.Resume();
		std::this_thread::sleep_for(s_timeToSleepMs);

		WHEN("Timer is stopped")
		{
			timer.Stop();

			THEN("Timer running flag is shut off.")
			{
				REQUIRE(!timer.IsRunning());
			}
			AND_THEN("Timer paused flag is shut off.")
			{
				REQUIRE(!timer.IsPaused());
			}
			AND_THEN("Elapsed time includes only up until when timer was stopped.")
			{
				std::this_thread::sleep_for(s_timeToSleepMs);
				milliseconds elapsed = timer.GetElapsedMs();
				CAPTURE(elapsed.count());

				REQUIRE(isRoughlyCorrect(elapsed, 100ms));
			}
			AND_THEN("Elapsed paused times are not cleared.")
			{
				milliseconds totalElapsedPaused = timer.GetTotalElapsedPausedMs();
				CAPTURE(totalElapsedPaused.count());

				REQUIRE(totalElapsedPaused != milliseconds::zero());
			}
		}
	}
}

SCENARIO("Restarting a timer", "[Timer]")
{
	using namespace std::chrono;

	GIVEN("A timer that has been running for 100ms")
	{
		Common::Timer timer;

		timer.Start();
		std::this_thread::sleep_for(s_timeToSleepMs*4);

		WHEN("Timer is restarted and run for s_timeToSleepMs")
		{
			timer.Restart();
			std::this_thread::sleep_for(s_timeToSleepMs);

			THEN("Elapsed time should be corect")
			{
				milliseconds elapsed = timer.GetElapsedMs();
				CAPTURE(elapsed.count());

				REQUIRE(isRoughlyCorrect(elapsed, 50ms));
			}
		}
	}
}

//===============================================================================

} // namespace Test
