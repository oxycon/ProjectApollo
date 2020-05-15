
#pragma once

// Connectivity:
// - Connect 5V from Arduino to VCC (Power connector) on Gasboard 7500E 
// - Connect GND from Arduino to GND (Power connector) on Gasboard 7500E 
// - Connect PIN 2 (Arduino RX) with TX (IO connector) on Gasboard 7500E 
// - Connect PIN 3 (Arduino TX) with RX (IO connector) on Gasboard 7500E 
// - Connect PIN digital GND Arduino with GND (IO connector) on Gasboard 7500E 
// 
// - Connect Vin from Arudino to 12V(+) on ULN2003 board and to (+) of 2-way valve motor and to (+) of 5-way valve motor
// - Connect GND from Arduino to GND(-) on ULN2003 board 
// - Connect PIN 6 pin from Arduino to IN PIN 1 on ULN2003 board
// - Connect PIN 7 pin from Arduino to IN PIN 2 on ULN2003 board
// - Connect PIN 8 pin from Arduino to IN PIN 3 on ULN2003 board
// - Connect OUT PIN 1 pin from ULN2003 board to (-) of 2-way valve
// - Connect OUT PIN 2 pin from ULN2003 board to (-) of 5-way valve
// - Connect OUT PIN 3 pin from ULN2003 board to (-) of relief valve
// - Connect OUT PIN 5 pin to common VCC(+) for all the consumers above. 
//
// - Connect Analog pin A0 to center potentiometer for adjusting timing 5-way valve timing
// - Connect GND and 5V to the two other ends of the potentiometer
//
// - Connect Analog pin A1 to center potentiometer for adjusting timing 2-way valve timing
// - Connect GND and 5V to the two other ends of the potentiometer
//
// - Connect Analog pin A2 to pushbutton switch to trigger relief valve and to pull-up resistor of value ~ 10K to the 5V rail 
// - Connect GND to the other end of the pushbutton switch
//
// - Program Arduino board
// - Verify that the LEDs blink in the right order
// - Connect 12V supply to Arduino (at least 1.5A current)
// - Verify that valves couple in the right order 


// PIN identifiers

// 
// PIN 6 on Arduino Uno/Nano
// - To be connnected to INPUT PIN 1 on ULN2003 board 
// - Which drives OUT PIN 1 on ULN2003 board 
// - Which drives (-) on 2-way valve
// 
const int valve_2way = 6;

// 
// PIN 7 on Arduino Uno/Nano
// - To be connnected to INPUT PIN 2 on ULN2003 board 
// - Which drives OUT PIN 2 on ULN2003 board 
// - Which drives (-) on 5-way valve
// 
const int valve_5way = 7;

// 
// PIN 8 on Arduino Uno/Nano
// - To be connnected to INPUT PIN 3 on ULN2003 board 
// - Which drives OUT PIN 3 on ULN2003 board 
// - Which drives (-) on the relief valve
// 
const int relief_valve = 8;

// 
// PIN 2 on Arduino Uno/Nano
// - Soft UART RX
// - To be connnected to TX on the Gasboard 7500E board
// 
const int  O2SENS_RX_PIN = 2;

// 
// PIN 3 on Arduino Uno/Nano
// - Soft UART TX
// - To be connnected to RX on the Gasboard 7500E board
// 
const int  O2SENS_TX_PIN = 3;

// 
// PIN A0 on Arduino Uno/Nano
// - Analog input - potentiometer to manually adjustg timing for 5-way valve
// - To be connnected to a potentiometer biased between 0V and 5V
// 
const int  ANALOG_5WAY_VALVE_ADJUST_PIN = A0;

// 
// PIN A1 on Arduino Uno/Nano
// - Analog input - potentiometer to manually adjustg timing for 2-way valve
// - To be connnected to a potentiometer biased between 0V and 5V
// 
const int  ANALOG_2WAY_VALVE_ADJUST_PIN = A1;

// 
// PIN A2 on Arduino Uno/Nano
// - Analog input - pushbutton to trigger the relief valve
// - To be connnected to a pushbutton that brings this value to ground on pushing. 
// - Pull resistor 10K to keep it high normally
// 
const int  ANALOG_RELIEF_VALVE_TRIGGER_BUTTON = A2;
