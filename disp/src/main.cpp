#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

const int DHT_PIN = 2;

int lcdColumns = 20;
int lcdRows = 4;

DHTesp temp;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  
  Serial.begin(115200);
  temp.setup(DHT_PIN, DHTesp::DHT22);
 
  lcd.init();                     
  lcd.backlight();
}

void loop(){

  // Sensor de Temperatura
  TempAndHumidity  data = temp.getTempAndHumidity();
  Serial.println("Temperatura: " + String(data.temperature, 2) + "°C");
  Serial.println("Umidade: " + String(data.humidity, 1) + "%");
  Serial.println("---------------");

  // Tela LCD
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: " + String(data.temperature, 2));
  lcd.setCursor(1, 2);
  lcd.println("Umidade: " + String(data.humidity, 1));


  delay(5000);
}