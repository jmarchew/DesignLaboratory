#include <BH1750.h>
#include <Wire.h>
#include <DHT.h>

BH1750 lightMeter;
DHT dht(26,DHT11);
void setup() {
  dht.begin();
  Serial.begin(9600);


  Wire.begin(); //I2C comunication SCL pin D22 SDA pin D21
 

  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  float lux = lightMeter.readLightLevel();
  
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
}
