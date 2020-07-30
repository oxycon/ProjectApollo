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

void setup()
{
  #ifdef O2SENSE_NEED_METADATA
  const uint8_t cmd_vernum[] = {O2SENSE_CMD_VERSIONNUMBER};
  const uint8_t cmd_sernum[] = {O2SENSE_CMD_SERIALNUMBER};
  #endif

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

void loop()
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
