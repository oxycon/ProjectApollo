//
// Gasboadd 7500E O2 sensor
//

#include <gasboard7500E.h>
#include <SoftwareSerial.h>

#define O2SENS_RX_PIN 2 // RX on Arduino, TX on Sensor
#define O2SENS_TX_PIN 3 // TX on Arduino, RX on Sensor

SoftwareSerial softSer = SoftwareSerial(O2SENS_RX_PIN, O2SENS_TX_PIN);

#ifdef O2SENSE_NEED_METADATA
bool has_sernum = false;
bool has_vernum = false;
#endif

#define OUTPUT_BUFFER_SIZE 64
char output_buffer[OUTPUT_BUFFER_SIZE];

void setup_o2sensor()
{
  #ifdef O2SENSE_NEED_METADATA
  const uint8_t cmd_vernum[] = {O2SENSE_CMD_VERSIONNUMBER};
  const uint8_t cmd_sernum[] = {O2SENSE_CMD_SERIALNUMBER};
  #endif

  delay(100);

  o2sens_init();
  Serial.begin(115200);
  pinMode(O2SENS_RX_PIN, INPUT);
  pinMode(O2SENS_TX_PIN, OUTPUT);
  softSer.begin(O2SENSE_BAUD_RATE);
  Serial.print("Hello ");
  delay(3000); // power on delay

  #ifdef O2SENSE_NEED_METADATA

  // I don't think these commands are working
  
  softSer.write((uint8_t*)cmd_vernum, 4);
  delay(500);
  softSer.write((uint8_t*)cmd_sernum, 4);
  delay(500);
  #endif
  Serial.println("World");
}

void loop_o2sensor()
{
  const int bytes_per_packet = 12;
  
  for(int i = 0; i < bytes_per_packet; i++)
  {
    if (softSer.available()) // at least 1 byte from UART arrived
    {
      o2sens_feedUartByte(softSer.read()); // give byte to the parser
      if (o2sens_hasNewData()) // a complete packet has been processed
      {
        o2sens_clearNewData(); // clear the new packet flag
  
        // display time in milliseconds for logging
        Serial.print(millis(), DEC); Serial.print(": ");
  
        // print out the whole buffer for debugging
        uint8_t* all_data = o2sens_getRawBuffer();
        for (int i = 0; i < all_data[1] + 3; i++)
        {
          sprintf(output_buffer, "%02X ", all_data[i]);
          Serial.print(output_buffer);
        }
  
        // print out the human readable data
        sprintf(output_buffer, ", %u , %u , %u"
          , o2sens_getConcentration16()
          , o2sens_getFlowRate16()
          , o2sens_getTemperature16()
          );
        Serial.println(output_buffer);
  
        if (has_sernum == false)
        {
          uint8_t* data = o2sens_getSerialNumber();
          if (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0 || data[4] != 0)
          {
            has_sernum = true;
            sprintf(output_buffer, "Serial Number: %02X %02X %02X %02X %02X"
              , data[0]
              , data[1]
              , data[2]
              , data[3]
              , data[4]
              );
            Serial.println(output_buffer);
          }
        }
        if (has_vernum == false)
        {
          char* data = o2sens_getVersionNumber();
          if (data[0] != 0) // first byte 
          {
            has_vernum = true;
            sprintf(output_buffer, "Version Number: %s", data);
            Serial.println(output_buffer);
          }
        }
      }
    }
  }
}

//
//    Valve cycling code for Project Apollo v1 prototype
//


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
// - Connect OUT PIN 3 pin from ULN2003 board to (-) of fan
// - Connect OUT PIN 5 pin to common VCC(+) for all the consumers above. 
// - Program Arduino board
// - Verify that the LEDs blink in the right order
// - Connect 12V supply to Arduino (at least 1.5A current)
// - Verify that valves couple in the right order 


// PIN identifiers

// 
// PIN 6 on Arduino Uno
// - To be connnected to INPUT PIN 1 on ULN2003 board 
// - Which drives OUT PIN 1 on ULN2003 board 
// - Which drives (-) on 2-way valve
// 
int valve_2way = 6;

// 
// PIN 7 on Arduino Uno
// - To be connnected to INPUT PIN 2 on ULN2003 board 
// - Which drives OUT PIN 2 on ULN2003 board 
// - Which drives (-) on 5-way valve
// 
int valve_5way = 7;

// 
// PIN 8 on Arduino Uno
// - To be connnected to INPUT PIN 3 on ULN2003 board 
// - Which drives OUT PIN 3 on ULN2003 board 
// - Which drives (-) on the FAN
// 
int fan_control = 8;

// Scale in which timing values are expressed (as multiplies of the number below)
int timing_unit_scale_milliseconds = 1000;

// Time for opening the 2-way valve
// 1 time units 
int timing_valve_2way = 2; 

// Time for alternating the 5-way valve
// 8 time units 
int timing_valve_5way_alternating = 7; 

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(valve_2way, OUTPUT);
  pinMode(valve_5way, OUTPUT);
  pinMode(fan_control, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // make the fan work all the time for now
  // TBD - add temperature-control driven fan (based on the temperature of the copper loop)
  digitalWrite(fan_control, HIGH);    

  // Setup O2 sensor
  setup_o2sensor();
}

void halfCycle(int state_5way)
{
  digitalWrite(LED_BUILTIN, state_5way);   // turn the built-in LED with the same value as 5-WAY state

  // Turn on 5-way
  digitalWrite(valve_5way, state_5way);    
  // Wait for 5-way to be opened for the given amount of time
  delay(timing_valve_5way_alternating * timing_unit_scale_milliseconds);

  // Turn on the 2-way briefly
  digitalWrite(valve_2way, HIGH);    
  delay(timing_valve_2way * timing_unit_scale_milliseconds);
  digitalWrite(valve_2way, LOW);    

  // Read O2 value
  loop_o2sensor();
}

void loop() {
  halfCycle(HIGH);
  halfCycle(LOW);  
}
