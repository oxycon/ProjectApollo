//
//    Valve cycling code for Project Apollo v1 prototype
//

#include <Arduino.h>

#include "apollo_board_pinout.h"
#include "valve_timing.h"
#include "debug_console.h"

// Initial values (for diagnostics only)
static int previousTiming2Way = 1000;
static int previousTiming5Way = 10000;

const int minTimingDelta_diagnostics = 100; // Eliminate debouncing - changes below 100 ms are not logged


int Get2WayValveTimingMilliseconds()
{
	// Between 0 and 1023
 	float potentiometerValue = (float)analogRead(ANALOG_2WAY_VALVE_ADJUST_PIN)/1023;
 	int minmax_timing_interval = max_timing_valve_2way_milliseconds - min_timing_valve_2way_milliseconds;
 	int timing_delta = potentiometerValue * minmax_timing_interval;
 	int currentTiming = min_timing_valve_2way_milliseconds + timing_delta;

 	if (abs(previousTiming2Way - currentTiming) > minTimingDelta_diagnostics)
 	{
 		DBG_println_float("- Timing adjusted for 2-way valve to ", (float)currentTiming/1000, " seconds", 3);

	 	previousTiming2Way = currentTiming;
 	}

 	return currentTiming;
}

int Get5WayValveTimingMilliseconds()
{
	// Between 0 and 1023
 	float potentiometerValue = (float)analogRead(ANALOG_5WAY_VALVE_ADJUST_PIN)/1023;
 	int minmax_timing_interval = max_timing_valve_5way_milliseconds - min_timing_valve_5way_milliseconds;
 	int timing_delta = potentiometerValue * minmax_timing_interval;
 	int currentTiming = min_timing_valve_5way_milliseconds + timing_delta;

 	if (abs(previousTiming5Way - currentTiming) > minTimingDelta_diagnostics)
 	{
 		DBG_println_float("- Timing adjusted for 5-way valve to ", (float)currentTiming/1000, " seconds", 3);

	 	previousTiming5Way = currentTiming;
 	}

 	return currentTiming;
}
