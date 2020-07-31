#ifndef VALVE_H
#define VALVE_H

extern uint8_t current_valve_states;

void valve_setup();

void set_valves(const uint8_t states, const uint8_t mask=0b11111111);
inline uint8_t get_valves() { return current_valve_states; }

bool set_valve(size_t n, bool state);
inline bool get_valve(size_t n) { return (current_valve_states >> n) & 1; }

#endif // VALVE_H
