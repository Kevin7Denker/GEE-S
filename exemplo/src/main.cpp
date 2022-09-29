#include <Arduino.h>
#include <api.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MQUnifiedsensor.h>

using namespace std;

#define DHTPIN 33     
#define DHTTYPE DHT22

#define MET_SENSOR 35

#define LED_WIFI 26
#define LED_SEND 25


/************************Hardware Related Macros************************************/
#define         Board                   ("ESP32")
/***********************Software Related Macros************************************/
#define         Type                    ("MQ-4") //MQ4
#define         Voltage_Resolution      (3.3)
#define         ADC_Bit_Resolution      (12) // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (4.4) //RS / R0 = 60 ppm 

DHT_Unified dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x3F,20,4);
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, MET_SENSOR, Type);

const char* ssid = "infoprojetos";
const char* password = "sistemas987";



API api("https://gees-f59cb-default-rtdb.firebaseio.com/","dados.json");

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}


float readMetano(){
  MQ4.update();
  float met = MQ4.readSensor();
  return met;
}

float readCO2(){
  return 0.0;
}

float readTemperature(){
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    return -1;
  }else{
    return event.temperature;
  }
}

float readHumidity(){
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));

    return -1;
  }
  else {
    return event.relative_humidity;
  }
}

void sendData(Sample sample){
  Serial.println("Sending...");
  Serial.println(String(sample.toJson().c_str()));
  digitalWrite(LED_SEND,HIGH);
  Response resp = api.send(sample);

  if(resp.getCode()==200){
    digitalWrite(LED_SEND,LOW);
  }

  Serial.println(resp.getMsg());
}



void setup() {

  Serial.begin(9600); 
  
  lcd.init();
  lcd.backlight();
  lcd.print("Leitor CO2");
  //incializando sensor temperatura
  dht.begin();

  MQ4.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ4.setA(1012.7); 
  MQ4.setB(-2.786); // Configure the equation to to calculate CH4 concentration
  //MQ4.setR0(3.86018237); // Value getted on calibration

  MQ4.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ4.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    Serial.print(".");
  }
  MQ4.setR0(calcR0/10);
  Serial.printf("  done!. %f",(calcR0/10));

  pinMode(MET_SENSOR,INPUT);

  pinMode(LED_WIFI,OUTPUT);
  pinMode(LED_SEND,OUTPUT);
  
  digitalWrite(LED_WIFI,LOW);
  digitalWrite(LED_SEND,LOW);


  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI,HIGH);
    delay(500);
    digitalWrite(LED_WIFI,LOW);
    Serial.print(".");
  }
  digitalWrite(LED_WIFI,HIGH);
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void updateLcd(Sample sample){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.printf("T:%0.2f U:%0.2f",sample.getTemperatura(),sample.getUmidade());
  lcd.setCursor(0,1);
  lcd.printf("M:%0.2f C:%0.2f",sample.getMetano(),sample.getCo2());
}

void loop() {
  string tempo;

  //TODO

  //READ TEMPERATURE AND HUMIDITY
  float temp = readTemperature();
  float humd = readHumidity();

  //READ METANO
  float metano = readMetano();

  //READ CO2
  float co2 = readCO2();

  Sample sample (co2,metano,temp,humd,to_string(millis()));

  updateLcd(sample);
  sendData(sample);

  delay(10000);

}


