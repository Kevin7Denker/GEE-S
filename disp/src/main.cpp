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
//#include "DS1307.h"
//Para utilizar NTP
#include <NTPClient.h>
#include <WiFiUdp.h>

#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) )



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

/************ Variáveis para o NTP **************/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


/************* Configuração da API *************/
API api("https://gees-f59cb-default-rtdb.firebaseio.com/","dados.json"); 

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
  return dynamic_cast<const Base*>(ptr) != nullptr;
}

/************* Leitor de Metano *************/
float LtMetano(){
  MQ4.init();
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
void envio(Sample sample){
  Serial.println("Enviando...");
  Serial.println(sample.toJson());
  Response resp = api.send(sample);
  //Serial.println(resp.getMsg());
   
}


// Based on https://github.com/PaulStoffregen/Time/blob/master/Time.cpp
// currently assumes UTC timezone, instead of using this->_timeOffset
String getFormattedDate() {
  unsigned long rawTime = (timeClient.getEpochTime()) / 86400L;  // in days
  unsigned long days = 0, year = 1970;
  uint8_t month;
  static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
    year++;
  rawTime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
  days=0;
  for (month=0; month<12; month++) {
    uint8_t monthLength;
    if (month==1) { // february
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    } else {
      monthLength = monthDays[month];
    }
    if (rawTime < monthLength) break;
    rawTime -= monthLength;
  }
  String monthStr = ++month < 10 ? "0" + String(month) : String(month); // jan is month 1  
  String dayStr = ++rawTime < 10 ? "0" + String(rawTime) : String(rawTime); // day of month  
  return dayStr+"/"+monthStr+"/"+String(year) + "T" + timeClient.getFormattedTime() + "Z";
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

/************* Data *************/
  
  /*clock.begin(); inicializar o Real Time Clock */
	/*clock.fillByYMD(??,?,??); necessita adicionar: Ano, Mês e Dia */  
	/*clock.fillByHMS(??,??,??); necessita adicionar: Hora, Minutos e Segundos */ 
	/*clock.fillDayOfWeek(SAT); necessita adicionar a tag do dia de ligamento */
	/*clock.setTime(); */

/************* Conectanto a Rede  *************/  
  WiFi.begin(ssid, password);     // Passando as inforamações para conexão
  Serial.println("Conectando...");
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  /******NTP ******/
  timeClient.begin();
  timeClient.setTimeOffset(-3*3600);
  /****************/

  Serial.println("");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP()); // Entrega o Endereço IP gerado

}


void updateLcd(Sample sample){ 
  
/************* Configurando Display  *************/   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(sample.getTimestamp());
  lcd.setCursor(0,1);
  lcd.printf("T:%0.2f U:%0.2f",sample.getTemp(),sample.getUmi());  // Exibe os valores de temperatura e umidade
  lcd.setCursor(0,2);
  lcd.printf("M:%0.2f C:-",sample.getMet());   // Exibe os valores de metano e CO2
}

void loop(){

  // Captura e envio do horario:
  // clock.getTime(); 
  // String tempo = clock.hour, ":", clock.minute, ":", clock.second; 

  /*Atualizando NTP*/
  timeClient.update();
  String formattedDate = getFormattedDate();


/************* Execução *************/  
  
  float temperatura = LtTemp();  // Armazena o valor de temperatura
  float umidade = LtUmidade();   // Armazena o valor de umidade
  float metano = LtMetano();     // Armazena o valor de metano
  float CO2 = LtCO2();           // Armazena o valor de co2
  
  // Trocar de to_string(millis) para a variavel tempo;
  Sample sample(CO2, metano, temperatura, umidade, formattedDate);  // Cria a amostra para envio

  updateLcd(sample);             // Atualiza o display com os valores atuais
  envio(sample);                 // Envia a amostra para a Api

  delay(10000);                  // Define de quanto em quanto tempo os dados serão atualizados

}

