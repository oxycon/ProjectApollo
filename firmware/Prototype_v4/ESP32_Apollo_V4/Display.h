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

#ifndef DISPLAY_H
#define DISPLAY_H

void display_setup();
void set_display_brightness(uint16_t value);
void display_boot_screen();
void display_config_screen();
void display_wifi_screen();
void display_main_screen_start();
void display_main_screen_update();
size_t getTouchCalibrationJson(char* buffer, size_t bSize=1<<30);


#endif // DISPLAY_H
