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

#ifndef OXYGEN_SENSOR_H
#define OXYGEN_SENSOR_H

bool o2_sensor_setup();
void o2_sensor_run();
size_t o2_sensor_data2csv(char* buffer, size_t bSize=1<<30);

extern float o2s_concentration;
extern float o2s_flow;
extern float o2s_temperature;

#endif OXYGEN_SENSOR_H
