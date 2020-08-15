led <0|1|on|off|true|false>            Set LED state
valve <n> [0|1|on|off|true|false]      Set or get valve state
valve-drivers [count]                  Number of installed DRV-8806 valve driver chips
concentrator [0|1|on|off|true|false]   Enable or disable concentrator cycle
cycle-duration <cycle> [miliseconds]   Set or get the duration of a cycle
cycle-valves <cycle> [valves]          Set or get cycle valve state bit-map
cycle-valve-mask <mask>                Set or get bit-masks of which valves should switch during cycles
oxygen                                 Get reults of last oxygen sensor measurements
color                                  Get reults of last color sensor measurements
adr-ambient                            Set or get the address of the ambient humidity, temperture, pressure sensor
adr-intake                             Set or get the address of the intake humidity, temperture sensor
adr-desiccant                          Set or get the address of the desiccant humidity, temperture sensor
adr-output                             Set or get the address of the output humidity, temperture sensor
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
