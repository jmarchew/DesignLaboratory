#include <DHT.h>

DHT dht(26,DHT11);
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  delay(1000);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  float temp =dht.readTemperature();
  float humidity= dht.readHumidity();

 Serial.printf("Temperatura: %.2f \n", temp);
 Serial.printf("Wilgotnosc: %.2f\n", humidity);
 delay(10000);
}
