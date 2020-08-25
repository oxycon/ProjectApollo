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
