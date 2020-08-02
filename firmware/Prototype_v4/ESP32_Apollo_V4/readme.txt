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
cycle-duration <cycle> [miliseconds]   Set or get the duration of a cycle
cycle-valves <cycle> [valves]          Set or get cycle valve state bit-map
cycle-valve-mask <mask>                Set or get bit-masks of which valves should switch during cycles
oxygen                                 Get reults of last oxygen sensor measurements
o2s-enable [0|1|on|off|true|false]     Enable or disable oxygen sensor measurements
o2s-period <milliseconds>              Set or get duration between oxygen sensor measurements
debug [0|1|on|off|true|false]          Enable or disable debug logging
wifi-enabled [0|1|on|off|true|false]   Enable or disable WIFI on next restart
ssid                                   Set or get WIFI SSID
wifi-password                          Set or get WIFI password
wifi-ip                                Set or get fixed WIFI IP address
dns                                    Set or get fixed WIFI DNS
gateway                                Set or get fixed WIFI gateway
subnet                                 Set or get fixed WIFI subnet
save                                   Save current configuration to FLASH
load                                   Restore configuration from FLASH
ip                                     Get local-IP address
mac                                    Get MAC address
time                                   Get current time
restart                                Restart the controller
help                                   Print help
?                                      Print help

