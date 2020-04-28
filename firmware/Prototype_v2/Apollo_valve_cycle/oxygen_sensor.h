//
// Gasboadd 7500E O2 sensor
//

#pragma once 

#define O2SENS_RX_PIN 2 // RX on Arduino, TX on Sensor
#define O2SENS_TX_PIN 3 // TX on Arduino, RX on Sensor

void setup_o2sensor();
void loop_o2sensor();
