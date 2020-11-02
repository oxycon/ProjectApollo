/*
 *   ESP32 Oxygen Concentrator
 *  ===========================
 * 
 * This software is provided "as is" for educational purposes only. 
 * No claims are made regarding its fitness for medical or any other purposes. 
 * The authors are not liable for any injuries, damages or financial losses.
 * 
 * Use at your own risk!
 * 
 * License: MIT https://github.com/oxycon/ProjectApollo/blob/master/LICENSE.txt
 * For more information see: https://github.com/oxycon/ProjectApollo
 */

#ifndef VALVE_H
#define VALVE_H

extern volatile uint8_t current_valve_states;
extern volatile uint8_t valve_alarms;

void valve_setup();

void set_valves(const uint8_t states, const uint8_t mask=0b11111111);
inline uint8_t get_valves() { return current_valve_states; }

void set_valve(size_t n, bool state);
inline bool get_valve(size_t n) { return (current_valve_states >> n) & 1; }

void set_valve_driver_count(size_t n);

extern volatile uint8_t valve_alarms;

#endif // VALVE_H
