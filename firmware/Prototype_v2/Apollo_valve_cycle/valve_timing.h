//
//    Valve cycling code for Project Apollo v1 prototype
//

#include "apollo_board_pinout.h"
#include "debug_console.h"


// Min time for opening the 2-way valve
const int min_timing_valve_2way_milliseconds = 100; 

// Min time for alternating the 5-way valve
const int min_timing_valve_5way_milliseconds = 100; 

// Max time for opening the 2-way valve
const int max_timing_valve_2way_milliseconds = 2 * 1000;

// Max time for alternating the 5-way valve
const int max_timing_valve_5way_milliseconds = 20 * 1000;



int Get2WayValveTimingMilliseconds();
int Get5WayValveTimingMilliseconds();
