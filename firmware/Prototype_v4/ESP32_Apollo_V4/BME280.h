// Bosch Temperature, Humidity, Pressure (BMP280), VOC (BME680) Sensors

#ifndef BME280_H
#define BME280_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


class Bme {
public:
  Bme() : bmeStatus(0), temperature(0.0), pressure(0.0), humidity(0.0), next_read_ms(0), delay_ms(1000) {};
  bool begin(uint8_t i2cAddr = BME280_ADDRESS);
  void run();
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);
  inline bool getStatus() { return bmeStatus; }
  inline float getTemperature() { return temperature; }
  inline float getPressure() { return pressure; }
  inline float getHumidity() { return humidity; }

  static void setup();

  size_t delay_ms;

protected:
  Adafruit_BME280 bme;
  bool bmeStatus;
  float temperature;
  float pressure;
  float humidity;
  size_t next_read_ms;
};

extern Bme bme280_1;
extern Bme bme280_2;

#endif // BME280_H
