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

Type help or ? to get a list of commands.
