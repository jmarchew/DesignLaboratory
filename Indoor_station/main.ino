#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <DHT.h>

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

const int IO = 27;    // DAT
const int SCLK = 14;  // CLK
const int CE = 26;    // RST
const int DHTout = 25; //DHT

//variables
char datetime[20];
char display_temperature[20];
char display_humidity[20];
float temperature;
float humidity;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);  
DHT dht(DHTout,DHT11);

void setup(){
  Rtc.Begin();
  lcd.init();                   
  lcd.backlight();
  dht.begin();
  delay(1000);
}

void loop(){

  temperature=dht.readTemperature();
  sprintf(display_temperature,"Temperature: %.1f\xDF""C",temperature);     //0xDF - degree sign
  humidity=dht.readHumidity();
  sprintf(display_humidity,"Humidity: %.1f%%",humidity);
  lcd.setCursor(0,1);
  lcd.print(display_temperature);
  lcd.setCursor(0,2);
  lcd.print(display_humidity);

  //continuously check datetime but read temp and humidity around every 10min
  for(int i = 0; i<600;i++){
    RtcDateTime dt = Rtc.GetDateTime();
    sprintf(datetime, "%02u/%02u/%04u %02u:%02u:%02u",
              dt.Month(),
              dt.Day(),
              dt.Year(),
              dt.Hour(),
              dt.Minute(),   
              dt.Second()+15 //RTC setup was compilation date, adding 15s adjusts time
                            // difference between compilation and flashing     
              );

    lcd.setCursor(0, 0);
    lcd.print(datetime);
  
    delay(1000);
  }
}
