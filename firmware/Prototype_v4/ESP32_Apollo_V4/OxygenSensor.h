#ifndef OXYGEN_SENSOR_H
#define OXYGEN_SENSOR_H

void o2_sensor_setup();
void o2_sensor_run();
size_t o2_sensor_data2csv(char* buffer, size_t bSize=1<<30);

extern float o2s_concentration;
extern float o2s_flow;
extern float o2s_temperature;

#endif OXYGEN_SENSOR_H
