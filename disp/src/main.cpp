#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHTesp.h"

#define DELAY 7000

//Pinagem
const int DHT_PIN = 2;

//Posições do display lcd (X & Y)
int lcdColumns = 20;
int lcdRows = 4;

//Sensores
DHTesp temp;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//Conexão
String serverName = "https://gees-f59cb-default-rtdb.firebaseio.com/dados.json";
unsigned long lastUpdate = 0;

void setup(){
  
  Serial.begin(115200);

  temp.setup(DHT_PIN, DHTesp::DHT22);
  
  // Id da rede e Senha para conexão
  char* ssid = "";
  char* password = "";

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  ssid = "null";
  password = "null";
  Serial.print("Conectado com Sucesso");

  lcd.init();                     
  lcd.backlight();
}

void loop(){

  long unsigned int now = millis();
  if((now - lastUpdate) > DELAY){
    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
      http.begin(serverName.c_str());
      http.addHeader("Content-Type", "application/json");
    }
  }

  // Sensor de Temperatura
  TempAndHumidity  data = temp.getTempAndHumidity();
  Serial.println("Temperatura: " + String(data.temperature, 2) + "°C");
  Serial.println("Umidade: " + String(data.humidity, 1) + "%");
  Serial.println("---------------");

  // Tela LCD
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: " + String(data.temperature, 2));
  lcd.setCursor(0, 2);
  lcd.println("Umidade: " + String(data.humidity, 1));


  delay(5000);
}