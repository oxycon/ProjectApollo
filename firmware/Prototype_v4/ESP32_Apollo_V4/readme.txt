----------------------------------------------------------------------------------------------

  The ESP32 started randomly crashing
 =====================================

/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/queue.c:1446 (xQueueGenericReceive)- assert failed!
abort() was called at PC 0x4008939b on core 1

Backtrace: 0x4008cf60:0x3ffbe690 0x4008d191:0x3ffbe6b0 0x4008939b:0x3ffbe6d0 0x400df875:0x3ffbe710 0x400d86cc:0x3ffbe730 0x400d6053:0x3ffbe750 0x400d60b5:0x3ffbe770 0x40080f89:0x3ffbe790 0x40081d95:0x3ffbe7b0 0x40085249:0x3ffbe7d0 0x4000bfed:0x3ffb1dd0 0x4008a7f1:0x3ffb1de0 0x40089ca4:0x3ffb1e00 0x400de844:0x3ffb1e30 0x400debcf:0x3ffb1e60 0x400d8859:0x3ffb1e80 0x400d88ad:0x3ffb1ea0 0x400d89bd:0x3ffb1ec0 0x400d7f3d:0x3ffb1ee0 0x400d81b8:0x3ffb1f10 0x400d8419:0x3ffb1f30 0x400d177b:0x3ffb1f50 0x400d59e9:0x3ffb1f70 0x400d4f7d:0x3ffb1f90 0x400e0fcd:0x3ffb1fb0 0x40089675:0x3ffb1fd0

0x4008cf60: invoke_abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp32/panic.c line 155
0x4008d191: abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp32/panic.c line 170
0x4008939b: xQueueGenericReceive at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/queue.c line 1446
0x400dfaf5: spiTransaction at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\esp32-hal-spi.c line 722
0x400d86bd: SPIClass::beginTransaction(SPISettings) at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\SPI\src\SPI.cpp line 135
0x400d6058: valve_update_spi() at C:\Users\caavenha\AppData\Local\Temp\arduino_build_850040\sketch\Valve.cpp line 68
0x400d60cd: set_valves(unsigned char, unsigned char) at C:\Users\caavenha\AppData\Local\Temp\arduino_build_850040\sketch\Valve.cpp line 82
0x40080f89: concentratorISR() at C:\Users\caavenha\AppData\Local\Temp\arduino_build_850040\sketch\Concentrator.cpp line 27
0x40081d95: __timerISR at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\esp32-hal-timer.c line 88
0x4008a5c6: vTaskEnterCritical at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/tasks.c line 4190
0x4008b054: xTaskResumeAll at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/tasks.c line 2224
0x40089ca7: xEventGroupWaitBits at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/event_groups.c line 428
0x400de86c: i2cProcQueue at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\esp32-hal-i2c.c line 1279
0x400debf7: i2cWrite at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\esp32-hal-i2c.c line 1621
0x400d8881: TwoWire::writeTransmission(unsigned short, unsigned char*, unsigned short, bool) at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\Wire\src\Wire.cpp line 133
0x400d88d5: TwoWire::endTransmission(bool) at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\Wire\src\Wire.cpp line 158
0x400d89e5: TwoWire::endTransmission() at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\Wire\src\Wire.cpp line 316
0x400d7f65: Adafruit_BME280::read24(unsigned char) at C:\Users\caavenha\Documents\Arduino\libraries\Adafruit_BME280_Library\Adafruit_BME280.cpp line 316
0x400d81e0: Adafruit_BME280::readTemperature() at C:\Users\caavenha\Documents\Arduino\libraries\Adafruit_BME280_Library\Adafruit_BME280.cpp line 407
0x400d8441: Adafruit_BME280::readHumidity() at C:\Users\caavenha\Documents\Arduino\libraries\Adafruit_BME280_Library\Adafruit_BME280.cpp line 468
0x400d177f: Bme::run() at C:\Users\caavenha\AppData\Local\Temp\arduino_build_850040\sketch\BME280.cpp line 32
0x400d59ed: sensor_run() at C:\Users\caavenha\AppData\Local\Temp\arduino_build_850040\sketch\SensorManager.cpp line 89
0x400d4f81: loop() at C:\Repos\ProjectApollo\firmware\Prototype_v4\ESP32_Apollo_V4/ESP32_Apollo_V4.ino line 69
0x400e0ff5: loopTask(void*) at C:\Users\caavenha\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\main.cpp line 19
0x40089675: vPortTaskWrapper at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port.c line 143


It is possible that it is not safe to call the SPI from the timer ISR in Concentrator.cpp
Essentially SPI_MUTEX_LOCK(); in esp32-hal-spi.c asserts sometimes. One possiblity could be to use software SPI for the valves.

A search also revealed:
https://github.com/espressif/arduino-esp32/issues/149 : hardware SPI is slower than software SPI by maybe a factor of 3 


For the time being SPI locks need to be disabled:
C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\esp32-hal-spi.c
#define CONFIG_DISABLE_HAL_LOCKS 1

----------------------------------------------------------------------------------------------

ESP32 Arduino install Video: https://www.youtube.com/watch?v=DgaKlh081tU&time_continue=61
  - Install Arduino IDE from https://www.arduino.cc/en/Main/Software
  - File => Preferences => Additional Boards Manager URLs: https://dl.espressif.com/dl/package_esp32_index.json
  - Tools => Board: "xxx" => Boards Manager => ESP32
  - Tools => Board: "xxx" => ESP32 Arduino => "WEMOS LOLIN32"
  - Tools => Serial Monitor => Baud: 115200

Install Library:
  - TFT_eSPI
  	!!! Edit Documents\Arduino\libraries\TFT_eSPI\User_Setup.h and configure ESP32 LCD SPI pins
  - TJpg_Decoder
  -ArduinoJson

Change WIFI credentials in Secrets.h

Control and Configuration:
There is an interactive command line interface that can be used with over the serial interface or TCP port 23.
Use telnet, PuTTY from https://putty.org or nc to connect using TCP.
Use Arduino IDE "tools=>Serial Monitor" or PuTTY from https://putty.org to connect using the USB serial port.

Type help or ? to get a list of CLI commands.

led <0|1|on|off|true|false>            Set LED state
valve <n> [0|1|on|off|true|false]      Set or get valve state
valve-drivers [count]                  Number of installed DRV-8806 valve driver chips
concentrator [0|1|on|off|true|false]   Enable or disable concentrator cycle
cycle-duration <cycle> [miliseconds]   Set or get the duration of a specifc cycle
cycle-durations [miliseconds] [...]    Set or get the cycle durations
cycle-valves <cycle> [valves]          Set or get cycle valve state bit-map
cycle-valve-mask <mask>                Set or get bit-masks of which valves should switch during cycles
oxygen                                 Get results of last oxygen sensor measurements
pressure                               Get results of last pressure sensor measurements
humidity                               Get results of last humidity sensor measurements
temperature                            Get results of last temperature sensor measurements
color                                  Get results of last color sensor measurements
errors                                 Get error log
adr-ambient [address]                  Set or get the address of the ambient humidity, temperture, pressure sensor
adr-intake [address]                   Set or get the address of the intake humidity, temperture sensor
adr-desiccant [address]                Set or get the address of the desiccant humidity, temperture sensor
adr-output [address]                   Set or get the address of the output humidity, temperture sensor
adr-in-pressure [address]              Set or get the address of the intake pressure sensor
adr-color [address]                    Set or get the address of the color sensor
adr-out-pressure [address]             Set or get the address of the output pressure sensor
debug [0|1|on|off|true|false]          Enable or disable debug logging
wifi-enabled [0|1|on|off|true|false]   Enable or disable WIFI on next restart
ssid                                   Set or get WIFI SSID
wifi-password                          Set or get WIFI password
wifi-ip                                Set or get fixed WIFI IP address
dns                                    Set or get fixed WIFI DNS
gateway                                Set or get fixed WIFI gateway
subnet                                 Set or get fixed WIFI subnet
time-format                            Set or get format for time representation
date-format                            Set or get format for date representation
save                                   Save current configuration to FLASH
load                                   Restore configuration from FLASH
config                                 Return configuration as JSON
data                                   Return current sensor data as JSON
ip                                     Get local-IP address
mac                                    Get MAC address
time                                   Get current time
timezone [time zone]                   Set or get the local time zone
brighness [value]                      Set or get the display brightness 0 to 100
restart                                Restart the controller
help                                   Print help
?                                      Print help
