
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

uint8_t current_valve_states = 0;

SPIClass valveSpi(VSPI);  //SPI1
SPISettings valveSpiSettings(VALVE_SPI_FREQUENCY, MSBFIRST, SPI_MODE0);
    
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
  uint8_t result = valveSpi.transfer(current_valve_states);
  valveSpi.endTransaction();
  // TODO: Check DRC-8806 alarms
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
