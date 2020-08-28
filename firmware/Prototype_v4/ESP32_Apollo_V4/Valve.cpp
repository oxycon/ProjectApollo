
#include "Arduino.h"
#include "SPI.h"

#include "Hardware.h"
#include "Config.h"
#include "Valve.h"


// Maps valve numbers to pins.
const int8_t valve_pin_map[] PROGMEM {
  VALVE_0_PIN,
  VALVE_1_PIN,
  VALVE_2_PIN,
  VALVE_3_PIN,
  VALVE_4_PIN
};

volatile uint8_t current_valve_states = 0;
volatile uint8_t valve_alarms = 0;


static SPIClass valveSpi(HSPI);  // SPI channel. VSPI is used by LCD.
static SPISettings valveSpiSettings(VALVE_SPI_FREQUENCY, MSBFIRST, SPI_MODE0);


/* 
 
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

But a search also revealed:
https://github.com/espressif/arduino-esp32/issues/149 : hardware SPI is slower than software SPI by maybe a factor of 3 


For the time being SPI locks need to be disabled:
C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\cores\esp32\esp32-hal-spi.c
#define CONFIG_DISABLE_HAL_LOCKS 1

*/

    
void valve_setup() {
  if (config.concentrator.drv8806_count == 0) {
    // No driver chips installed. Drive valves through GPIOs directly.
    for (size_t n=0; n<sizeof(valve_pin_map); n++) {
      pinMode(valve_pin_map[n], OUTPUT);
      digitalWrite(valve_pin_map[n], LOW);
    }
  } else {
    pinMode(VALVE_SCK_PIN, OUTPUT);
    digitalWrite(VALVE_SCK_PIN, LOW);
    pinMode(VALVE_MOSI_PIN, OUTPUT);
    digitalWrite(VALVE_MOSI_PIN, LOW);
    if (VALVE_MISO_PIN > -1) {
      pinMode(VALVE_MISO_PIN, OUTPUT);
      digitalWrite(VALVE_MISO_PIN, INPUT_PULLUP);
    }
    pinMode(VALVE_LATCH_PIN, OUTPUT);
    digitalWrite(VALVE_LATCH_PIN, LOW);
    if (VALVE_ALARM_PIN > -1) {
      pinMode(VALVE_ALARM_PIN, OUTPUT);
      digitalWrite(VALVE_ALARM_PIN, INPUT_PULLUP);
    }

    valveSpi.begin(VALVE_SCK_PIN, VALVE_MISO_PIN, VALVE_MOSI_PIN, VALVE_LATCH_PIN);
    valveSpi.setHwCs(true);
    //  Output Edge: Falling / Data Capture: Rising
    valveSpi.setDataMode(SPI_MODE0); 
    valveSpi.setFrequency(VALVE_SPI_FREQUENCY);
  }
}

void set_valve_driver_count(size_t n)
{
  if (n == 0 || config.concentrator.drv8806_count != 0) {
    valveSpi.end();
  }
  config.concentrator.drv8806_count = n;
  valve_setup();
}

void valve_update_spi() {
  valveSpi.beginTransaction(valveSpiSettings);
  valve_alarms = valveSpi.transfer(current_valve_states);
  valveSpi.endTransaction();
}

void set_valves(const uint8_t states, const uint8_t mask/*=0b11111111*/) {
  current_valve_states = (current_valve_states & ~mask) | states;
  if (config.concentrator.drv8806_count == 0) {
    // No driver chips installed. Drive valves through GPIOs directly.
    for (size_t n=0; n<sizeof(valve_pin_map); n++) {
      digitalWrite(valve_pin_map[n], (current_valve_states >>n) & 1 );    
    }
  } else {
    return valve_update_spi();
  }    
}

void set_valve(size_t n, bool state) {
  DEBUG_printf(FS("Set Valve %d to %d\n"), n, state);
  current_valve_states = (current_valve_states & ~(1<<n)) | (((uint8_t) state) << n);
  if (config.concentrator.drv8806_count == 0) {
    // No driver chips installed. Drive valves through GPIOs directly.  
    digitalWrite(valve_pin_map[n], (current_valve_states >>n) & 1 );  
  } else {
    return valve_update_spi();
  }
}
