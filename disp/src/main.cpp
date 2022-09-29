/************* Bibliotecas *************/ 
#include <Arduino.h>
#include <api.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <MQUnifiedsensor.h>
#include "DHTesp.h"

/************* User *************/ 
using namespace std;

/************************ Configuração ************************************/
#define         Board                   ("ESP32") // definição do modelo de placa
#define         Type                    ("MQ-4")  // definição do modelo do sensor
#define         Voltage_Resolution      (3.3)     // Voltagem do sensor 
#define         ADC_Bit_Resolution      (12)      // Configuração da Resolução
#define         RatioMQ4CleanAir        (4.4)     // R0 = 60 ppm 


/************* Pinagem *************/
#define MET_SENSOR 35
const int DHT_PIN = 33;

/************* Componentes *************/

DHTesp dht; // Sensor de Umidade e temperatura
LiquidCrystal_I2C lcd(0x3f, 20, 4); // Display lcd
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, MET_SENSOR, Type); // Sensor de Metano

/************* Configuração de Rede *************/
const char* ssid = "infoprojetos";    // Nome da rede
const char* password = "sistemas987"; // Senha da rede

/************* Configuração da API *************/
API api("https://gees-f59cb-default-rtdb.firebaseio.com/","dados.json"); 

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
  return dynamic_cast<const Base*>(ptr) != nullptr;
}

/************* Leitor de Metano *************/
float LtMetano(){
  MQ4.update();
  float met = MQ4.readSensor();
  return met; 
}

/************* Leitor de Temperatura *************/
float LtTemp(){
  float temp = dht.getTemperature();
  return temp;
}

/************* Leitor de Umidade *************/
float LtUmidade(){
  float umi = dht.getHumidity();
  return umi;
}

/************* Leitor de Umidade *************/
float LtCO2(){
  return 0.0;
}

/************* Envio dos Dados *************/
float envio(Sample sample){
  Serial.println("Enviando...");
  Serial.println(String(sample.toJson().c_str()));
  Response resp = api.send(sample);
  Serial.println(resp.getMsg());
}

void setup(){
  
  Serial.begin(9600);

/************* Inicializando Componentes  *************/
  lcd.init();                         // Display LCD
  lcd.backlight();                    // Backlight do display
  dht.setup(DHT_PIN, DHTesp::DHT22);  // Sensor de umidade e temperatura

 /******** Configurando ********/
  MQ4.setRegressionMethod(1); 
  MQ4.setA(1012.7); 
  MQ4.setB(-2.786); // Configura a equação para calcular a concentração de CH4
  MQ4.init();       // Sensor de metano
 
 /******** Calibração ********/
  Serial.print("Calibrando...");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ4.update(); // 
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    Serial.print(".");
  }
  MQ4.setR0(calcR0/10);
  Serial.printf("  done!. %f",(calcR0/10));

/************* Conectanto a Rede  *************/  
  WiFi.begin(ssid, password);     // Passando as inforamações para conexão
  Serial.println("Conectando...");
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP()); // Entrega o Endereço IP gerado

}

void loop(){

  string tempo; // Precisa adicionar o RTC

/************* Execução *************/  
  
  float temperatura = LtTemp();  // Armazena o valor de temperatura
  float umidade = LtUmidade();   // Armazena o valor de umidade
  float metano = LtMetano();     // Armazena o valor de metano
  float CO2 = LtCO2();           // Armazena o valor de co2
  
  Sample sample(CO2, metano, temperatura, umidade, to_string(millis()));  // Cria a amostra para envio

  updateLcd(sample);             // Atualiza o display com os valores atuais
  envio(sample);                 // Envia a amostra para a Api

  delay(10000);                  // Define de quanto em quanto tempo os dados serão atualizados
}


void updateLcd(Sample sample){ 
  
/************* Configurando Display  *************/   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.printf("T:%0.2f U:%0.2f",sample.getTemp(),sample.getUmi());  // Exibe os valores de temperatura e umidade
  lcd.setCursor(0,1);
  lcd.printf("M:%0.2f C:%0.2f",sample.getMet(),sample.getCo2());   // Exibe os valores de metano e CO2
}