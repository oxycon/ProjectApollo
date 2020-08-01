#ifndef OXYGEN_SENSOR_H
#define OXYGEN_SENSOR_H

void o2_sensor_setup();
void o2_sensor_enable(bool state);
void o2_sensor_run();
size_t o2_sensor_data2csv(char* buffer, size_t bSize=1<<30);

#endif OXYGEN_SENSOR_H
