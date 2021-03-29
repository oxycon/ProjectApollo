#include "PulseOximeterService.h"

#include <cstring>
#include <string>
#include <time.h>

#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <freertos/task.h>

//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

static const char* LOG_TAG = "PulseOximeterService";
static const char* PULSE_OXIMETER_NAME = "BerryMed";
static const char* DEVICE_NAME = "Apollo";

static const uint8_t BLE_SCAN_INTERVAL_MSEC = 1500;
static const uint8_t BLE_SCAN_WINDOW_MSEC = 500;
static const uint8_t BLE_SCAN_DURATION_SEC = 5;

static const std::string PulseOximeterServiceStatusString[]{
    "NotStarted",
    "ReadyToScan",
    "Scanning",
    "FoundDevice",
    "Connecting",
    "Connected",
    "Disconnected"};

static BLEUUID serviceUUID("49535343-FE7D-4AE5-8FA9-9FAFD205E455");
static BLEUUID charUUID("49535343-1E4D-4BD9-BA61-23C647249616");

PulseOximeterService& PulseOximeterService::Instance()
{
    // Singleton Instance of PulseOximeterService
    static PulseOximeterService instance;
    return instance;
}

PulseOximeterService::PulseOximeterService() {}

PulseOximeterService::~PulseOximeterService() {}

static void RemoteCharacteristicNotifyCallback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData,
    size_t length, bool isNotify)
{
    ESP_LOGD(LOG_TAG, "Data received for Characteristic : %s Length : %d [0x%x 0x%x 0x%x 0x%x 0x%x]",
             pBLERemoteCharacteristic->getUUID().toString().c_str(),
             length,
             pData[0], pData[1], pData[2], pData[3], pData[4]);

    PulseOximeterReading reading = {};
    reading.Timestamp = xTaskGetTickCount();
    reading.IsFingerPresent = ((pData[2] & 0x10) == 0);
    reading.PulseRateBPM = pData[3] | (pData[2] & 0x40) << 1;
    reading.SpO2 = pData[4];
    reading.IsValid = (reading.SpO2 != 0x7F) && (reading.PulseRateBPM != 0x7F);
    if (reading.IsValid && reading.IsFingerPresent)
    {
        ESP_LOGI(LOG_TAG, "Timestamp : %ld Pulse : %d bpm Sp02 : %d %%", reading.Timestamp, reading.PulseRateBPM, reading.SpO2);
    }
    else
    {
        ESP_LOGD(LOG_TAG, "Timestamp : %ld Waiting for User");
    }
    PulseOximeterService::Instance().SetReading(reading);
}

class ClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient* m_pClientNoRef)
    {
        ESP_LOGI(LOG_TAG, "Client Connected");
    }

    void onDisconnect(BLEClient* m_pClientNoRef)
    {
        PulseOximeterService::Instance().SetStatus(PulseOximeterServiceStatus::Disconnected);
        ESP_LOGI(LOG_TAG, "Client Disconnected");
    }
};

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class AdvertisedDeviceCallback : public BLEAdvertisedDeviceCallbacks
{
    /**
   * Called for each advertising BLE server.
   */
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        std::string name = advertisedDevice.getName();
        if (name.rfind(PULSE_OXIMETER_NAME) == 0)
        {
            ESP_LOGI(LOG_TAG, "Found BerryMed Oximeter\n");
            BLEDevice::getScan()->stop();

            PulseOximeterService::Instance().SetDevice(advertisedDevice);
            PulseOximeterService::Instance().SetStatus(PulseOximeterServiceStatus::FoundDevice);
        }
        // Found our server
    } // onResult
};    // AdvertisedDeviceCallback

bool PulseOximeterService::Start()
{
    ESP_LOGD(LOG_TAG, "PulseOximeterService starting..");
    BLEDevice::init(DEVICE_NAME);

    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 5 seconds.
    BLEDevice::getScan()->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallback());
    BLEDevice::getScan()->setInterval(BLE_SCAN_INTERVAL_MSEC);
    BLEDevice::getScan()->setWindow(BLE_SCAN_WINDOW_MSEC);
    BLEDevice::getScan()->setActiveScan(true);

    SetStatus(PulseOximeterServiceStatus::Scanning);
    BLEDevice::getScan()->start(BLE_SCAN_DURATION_SEC, false);

    return true;
}

void PulseOximeterService::SetDevice(const BLEAdvertisedDevice& device)
{
    m_spAdvertisedDevice.reset(new BLEAdvertisedDevice(device));
}

bool PulseOximeterService::ConnectToServer()
{
    if (m_spAdvertisedDevice == nullptr)
    {
        ESP_LOGE(LOG_TAG, "Called connect to server without an advertised device");
        return false;
    }

    ESP_LOGI(LOG_TAG, "Forming a connection to %s", m_spAdvertisedDevice->getAddress().toString().c_str());

    m_pClientNoRef = BLEDevice::createClient();
    ESP_LOGI(LOG_TAG, "Created client");

    m_pClientNoRef->setClientCallbacks(new ClientCallback());

    // Connect to the remove BLE Server.
    m_pClientNoRef->connect(m_spAdvertisedDevice.get());
    ESP_LOGI(LOG_TAG, "Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteServiceNoRef = m_pClientNoRef->getService(serviceUUID);
    if (pRemoteServiceNoRef == nullptr)
    {
        ESP_LOGE(LOG_TAG, "Failed to find service UUID: %s", serviceUUID.toString().c_str());
        m_pClientNoRef->disconnect();
        return false;
    }
    ESP_LOGI(LOG_TAG, "Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE
    // server.
    BLERemoteCharacteristic* pRemoteCharacteristicNoRef = pRemoteServiceNoRef->getCharacteristic(charUUID);
    if (pRemoteCharacteristicNoRef == nullptr)
    {
        ESP_LOGE(LOG_TAG, "Failed to find our characteristic UUID: %s", charUUID.toString().c_str());
        m_pClientNoRef->disconnect();
        return false;
    }
    ESP_LOGI(LOG_TAG, "Found our characteristic");

    // Read the value of the characteristic.
    if (pRemoteCharacteristicNoRef->canRead())
    {
        std::string value = pRemoteCharacteristicNoRef->readValue();
        ESP_LOGI(LOG_TAG, "The characteristic value was: %s", value.c_str());
    }

    if (pRemoteCharacteristicNoRef->canNotify())
    {
        pRemoteCharacteristicNoRef->registerForNotify(
            RemoteCharacteristicNotifyCallback);
        ESP_LOGI(LOG_TAG, "Registering for data notification");
    }
    else
    {
        ESP_LOGI(LOG_TAG, "The remote characteristics does not support notification.");
    }

    SetStatus(PulseOximeterServiceStatus::Connected);
    return true;
}

void PulseOximeterService::SetStatus(PulseOximeterServiceStatus status)
{
    m_status = status;
    ESP_LOGI(LOG_TAG, "Setting PulseOximeterService Status to %s", PulseOximeterServiceStatusString[(int)status].c_str());

    // Reset reading if invalid state
    switch (status)
    {
    case PulseOximeterServiceStatus::NotStarted:
    case PulseOximeterServiceStatus::ReadyToScan:
    case PulseOximeterServiceStatus::Disconnected:
    case PulseOximeterServiceStatus::Scanning:
    case PulseOximeterServiceStatus::FoundDevice:
    case PulseOximeterServiceStatus::Connecting:
    {
        m_latestReading = {};
    }
    break;
    case PulseOximeterServiceStatus::Connected:
        break;
    }

}

PulseOximeterServiceStatus PulseOximeterService::GetStatus() const
{
    return m_status;
}

void PulseOximeterService::Tick()
{
    switch (GetStatus())
    {
    case PulseOximeterServiceStatus::NotStarted:
        break;
    case PulseOximeterServiceStatus::ReadyToScan:
    case PulseOximeterServiceStatus::Disconnected:
    case PulseOximeterServiceStatus::Scanning:
    {
        BLEDevice::getScan()->start(BLE_SCAN_DURATION_SEC, false);
    }
    break;

    case PulseOximeterServiceStatus::FoundDevice:
    {
        if (PulseOximeterService::Instance().ConnectToServer())
        {
            ESP_LOGI(LOG_TAG, "Connected to the Pulse Oximeter Server.");
            SetStatus(PulseOximeterServiceStatus::Connected);
        }
        else
        {
            ESP_LOGE(LOG_TAG, "Failed to connected the Pulse Oximeter Server. Retying...");
            SetStatus(PulseOximeterServiceStatus::ReadyToScan);
        }
    }
    break;
    case PulseOximeterServiceStatus::Connecting:
    case PulseOximeterServiceStatus::Connected:
        break;
    }
}

void PulseOximeterService::SetReading(const PulseOximeterReading& reading)
{
    m_latestReading = reading;
}

PulseOximeterReading PulseOximeterService::GetReading() const
{
    return m_latestReading;
}
