/************* Biblioteca *************/ 
#include "sample.h"

/************* Criação dos Valores da Amostra *************/

// É passado os 4 valores do tipo float captados pelos sensores;
// E também o horário em que os valores foram emitidos e captados; 
Sample::Sample(float co2,float metano,float temperatura,float umidade,String timestamp):co2(co2),metano(metano),temperatura(temperatura),umidade(umidade),timestamp(timestamp){}


/************* Criação do Arquivo Json para Envio dos Dados *************/ 

// Com a criação do arquivo json, podemos enviar os dados para nossa api
// Para podermos analisar os dados captados do inicio ao fim da execução
String Sample::toJson(){
    String out;

    out = "{";
    out += "\"co2\":"+String(co2)+",";
    out += "\"metano\":"+String(metano)+",";
    out += "\"temperatura\":"+String(temperatura)+",";
    out += "\"umidade\":"+String(umidade)+",";
    out += "\"timestamp\":\""+timestamp+"\"";
    out += "}";

    return out;
}

/************* Gets *************/ 
float Sample::getTemp(){
    return temperatura;
}

float Sample::getMet(){
    return metano;
}

float Sample::getUmi(){
    return umidade;
}

float Sample::getCo2(){
    return co2;
}

String Sample::getTimestamp(){
    return timestamp;
}

