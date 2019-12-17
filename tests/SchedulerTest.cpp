//---------------------------------------------------------------
//
// SchedulerTest.cpp
//

#include "SchedulerTest.h"

#include "Catch2/catch.hpp"

#include <chrono>

namespace Tests {

//===============================================================================

	// Test 1)
	// Create event that triggers in 50 ms
	// Ensure that the event callback is run


	SCENARIO("Trigger an event in 50ms", "[Scheduler]")
	{
		using namespace std::chrono;
		GIVEN("A scheduler and an event")
		{
			
		}
	}

	// Test 2)
	// Create event that triggers in 50ms
	// Cancel event
	// Ensure that the event was canceled

	// Test 3)
	// Create event that triggers in 50ms and costs 500ms to run
	// Behavior should be
		// 50 ms goes by
		// event begins 
		// 500ms goes by
		// event finally triggered

	// Test 4)
	// Create event that runs now at a 100ms interval
	// in 50 ms test runs
	// test runs again in 100ms
	// test runs again in 100ms
	// test is canceled

	// Test 5)
	// Create event that runs now at 100ms interval and has a delayed tick
	// event does not run immediately
	// in 100ms event will run

	// Test 6)
	// Create event that executes in 100ms
	// Kill the owner
	// Ensure that the callback does not execute

	// Test 7 ) (slow factor)
	// Create a test that triggers now, has a time cost of 250ms
	// after 150ms of time cost, a slow factor of 1.5x is applied
	// increase remaining time cost by 1.5 * remaining time cost
	// ensure the callback is executed in 150ms

	// Test 8) (speed factor)
	// Create a test that triggers in 100 ms and has a time cost of 2000ms
	// after 1000ms of time cost, a speed factor of 2x is applied
	// decrease remaining time cost by the speed factor (divide by 2x)
	// ensure that the callback is executed in 500ms

	// impl
	// when changing the time cost, if the time cost is less than the original
		// remove the function from the scheduler and add it back in the right place
	// I think this means that time cost needs to be changed by id from the scheduler

	// Test 9 Ensure ordering works
	// Create 5 events, first one starting in 1000 ms and each subsequent one
	// happening 100ms less apart from the last. Last one should start in 500ms
		// ensure the first event that runs was the last created
		// and then ensure the last event that runs was the first one created

//===============================================================================

}
