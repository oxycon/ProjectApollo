/**
 *   ESP32 & Arduino BLE Pulse Oximeter Service
 *  =============================
 * 
 * This software is provided "as is" for educational purposes only. 
 * No claims are made regarding its fitness for medical or any other purposes. 
 * The authors are not liable for any injuries, damages or financial losses.
 * 
 * Use at your own risk!
 * 
 * License: MIT https://github.com/oxycon/ProjectApollo/blob/master/LICENSE.txt
 * For more information see: https://github.com/oxycon/ProjectApollo
 */



#ifndef PULSE_OXIMETER_SERVICE
#define PULSE_OXIMETER_SERVICE

#ifdef ENABLE_PULSE_OXIMETER

#ifdef ARDUINO_ARCH_ESP32
#include <memory>

#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#else
#include <ArduinoBLE.h>
#endif

/**
 * @brief Repesents a reading from a Pulse Oximeter
 * 
 */
struct PulseOximeterReading
{
    long Timestamp;               // Unique timestamp of data
    bool IsValid = false;         // Valid if all information was parsed
    bool IsFingerPresent = false; // Is Finger present in device
    uint8_t PulseRateBPM = 0;     // Pulse rate in Beats per Minute
    uint8_t SpO2 = 0;             // Blood SpO2 percentage
};

/**
 * @brief Status of the Pulse Oximeter service status
 * 
 */
enum PulseOximeterServiceStatus
{
    NotStarted = 0,
    ReadyToScan,
    Scanning,
    FoundDevice,
    Connecting,
    Connected,
    Disconnected
};

/**
 * @brief Pulse Oximeter service
 *
 * Usage 
      #include "PulseOximeterService.h"

      void setup() {
        Serial.begin(115200);
        Serial.println("Starting Pulse Oximeter BLE Client application...");
        if (!PulseOximeterService::Instance().Start())
        {
          Serial.println("ERROR: Failed to start an PulseOximeterService. Check if device supports BLE");
        }
      } // End of setup.


      // This is the Arduino main loop function.
      void loop() {

        PulseOximeterService::Instance().Tick();
        if (PulseOximeterService::Instance().GetStatus() == PulseOximeterServiceStatus::Connected)
        {
          PulseOximeterReading reading = PulseOximeterService::Instance().GetReading();
        }
      } // End of loop
 *
 * 
 */
class PulseOximeterService
{
  public:
    /**
   * @brief Destroy the Pulse Oximeter Service object
   * 
   */
    virtual ~PulseOximeterService();

    /**
     * @brief Get the singleton instance of the Service
     * 
     * @return PulseOximeterService& 
     */
    static PulseOximeterService& Instance();

    /**
     * @brief Start the service
     * 
     * @return true 
     * @return false 
     */
    bool Start();
    void Stop();

    /**
     * @brief Set the Device object
     * 
     * @param device 
     */
#ifdef ARDUINO_ARCH_ESP32
    void SetDevice(const BLEAdvertisedDevice& device);
#else
    void SetDevice(const BLEDevice& device);
#endif

    /**
     * @brief Get the Status object
     * 
     * @return PulseOximeterServiceStatus 
     */
    PulseOximeterServiceStatus GetStatus() const;

    /**
     * @brief Set the Status object
     * 
     * @param status 
     */
    void SetStatus(PulseOximeterServiceStatus status);

    /**
     * @brief Set the Reading object
     * 
     * @param reading 
     */
    void SetReading(const PulseOximeterReading& reading);

    /**
     * @brief Get the Reading object
     * 
     * @return PulseOximeterReading 
     */
    PulseOximeterReading GetReading() const;

    /**
     * @brief Run tick of the loop. Call this in the arduino loop
     * 
     */
    void Tick();

  private:
    PulseOximeterService();
    PulseOximeterService(PulseOximeterService&) = delete;

    /**
     * @brief Connect to discovered device
     * 
     */
    bool ConnectToServer();

    bool m_isStarted = false;
    PulseOximeterServiceStatus m_status = PulseOximeterServiceStatus::NotStarted;
#ifdef ARDUINO_ARCH_ESP32
    std::unique_ptr<BLEAdvertisedDevice> m_spAdvertisedDevice;
    BLEClient* m_pClientNoRef = nullptr;
#else
    BLEDevice m_device;
#endif
    PulseOximeterReading m_latestReading = {};
};

#endif // ENABLE_PULSE_OXIMETER

#endif // PULSE_OXIMETER_SERVICE
