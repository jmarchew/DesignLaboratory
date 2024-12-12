#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>

// Sensor pins
const int DHTout = 25; // DHT sensor
DHT dht(DHTout, DHT11);
BH1750 lightMeter;

// ESP-NOW Data structure
typedef struct {
  float temperature;
  float humidity;
  float lux;
} SensorData;

SensorData sensorData;

// MAC address of the master device
uint8_t masterMacAddr[] = {0xa8, 0x42, 0xe3, 0xab, 0xa3, 0x98};

void setup() {
  // Initialize sensors
  dht.begin();
  Wire.begin();
  lightMeter.begin();

  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Register the master as a peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, masterMacAddr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.begin(115200);
}

void loop() {
  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float lux = lightMeter.readLightLevel();

  // Prepare data to send
  sensorData.temperature = temperature;
  sensorData.humidity = humidity;
  sensorData.lux = lux;

  // Send data to the master device
  esp_err_t result = esp_now_send(masterMacAddr, (uint8_t *)&sensorData, sizeof(sensorData));

  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  delay(10000); // Send data every 10 seconds
}
