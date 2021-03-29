#ifndef PULSE_OXIMETER_SERVICE
#define PULSE_OXIMETER_SERVICE

#include <memory>

#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

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
        esp_log_level_set("PulseOximeterService", ESP_LOG_VERBOSE);        // set all components to ERROR level
        PulseOximeterService::Instance().Start();
      } // End of setup.


      // This is the Arduino main loop function.
      void loop() {

        PulseOximeterService::Instance().Tick();
        delay(100); // Delay a second between loops.
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
    void SetDevice(const BLEAdvertisedDevice& device);

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

    std::unique_ptr<BLEAdvertisedDevice> m_spAdvertisedDevice;
    BLEClient* m_pClientNoRef = nullptr;
    PulseOximeterReading m_latestReading = {};
};
#endif // PULSE_OXIMETER_SERVICE
