#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

const int IO = 27;    // DAT
const int SCLK = 14;  // CLK
const int CE = 26;    // RST
const int DHTout = 25; // DHT

// Variables
char datetime[20];
char display_dht[20];
char display_row3[20];
char display_source[20];
float temperature;
float humidity;
float lux;

// ESP-NOW Data structure
typedef struct {
  float temperature;
  float humidity;
  float lux;
} SensorData;

SensorData receivedData;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);
DHT dht(DHTout, DHT11);
BH1750 lightMeter;

unsigned long lastSwitchTime = 0;
bool displayLocalData = true;

void onDataReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
}

void setup() {
  // Initialize LCD, RTC, DHT, BH1750, and Wi-Fi
  Rtc.Begin();
  lcd.init();                   
  lcd.backlight();
  dht.begin();
  Wire.begin(); // I2C communication
  lightMeter.begin();

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    lcd.setCursor(0, 0);
    lcd.print("ESP-NOW Init Fail");
    return;
  }

  // Register the receive callback function for ESP-NOW
  esp_now_register_recv_cb(onDataReceive);

  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();

  // Switch display data every 10 seconds
  if (currentTime - lastSwitchTime >= 10000) {
    displayLocalData = !displayLocalData;
    lastSwitchTime = currentTime;
  }

  if (displayLocalData) {
    // Read local sensor data
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    lux = lightMeter.readLightLevel();

    // Format and display local data
    sprintf(display_dht, "T: %.1f\xDF""C H: %.1f%%", temperature, humidity);
    if(lux<50){
      sprintf(display_row3, "Dark %.1f          ", lux);
    }
    else if(lux>=50 && lux<500){
      sprintf(display_row3, "Dusk %.1f          ", lux);
    }
    else if(lux>=500){
      sprintf(display_row3, "Daylight %.1f      ", lux);
    }
    sprintf(display_source, "Source: InDoor ");
  } else {
    // Format and display received data
    sprintf(display_dht, "T: %.1f\xDF""C H: %.1f%%", receivedData.temperature, receivedData.humidity);
    if(receivedData.lux<50){
      sprintf(display_row3, "Dark %.1f          ", receivedData.lux);
    }
    else if(receivedData.lux>=50 && receivedData.lux<500){
      sprintf(display_row3, "Dusk %.1f          ", receivedData.lux);
    }
    else if(receivedData.lux>=500){
      sprintf(display_row3, "Daylight %.1f      ", receivedData.lux);
    }
    sprintf(display_source, "Source: OutDoor");
  }

  lcd.setCursor(0, 1);
  lcd.print(display_dht);
  lcd.setCursor(0, 2);
  lcd.print(display_row3);
  lcd.setCursor(0, 3);
  lcd.print(display_source);

  // Display datetime
  RtcDateTime dt = Rtc.GetDateTime();
  sprintf(datetime, "%02u/%02u/%04u %02u:%02u",
          dt.Day(),
          dt.Month(),
          dt.Year(),
          dt.Hour(),
          dt.Minute()
          );

  lcd.setCursor(0, 0);
  lcd.print(datetime);

  delay(1000);
}