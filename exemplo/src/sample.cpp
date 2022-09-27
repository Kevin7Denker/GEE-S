#include "sample.h"

Sample::Sample(float co2,
               float metano, 
               float temperatura,
               float umidade,
               string timestamp):co2(co2),metano(metano),temperatura(temperatura),umidade(umidade),timestamp(timestamp){}

string Sample::toJson(){
    std::stringstream out;

    out << "{";
    out << "\"co2\":"<<co2<<",";
    out << "\"metano\":"<<metano<<",";
    out << "\"temperatura\":"<<temperatura<<",";
    out << "\"umidade\":"<<umidade<<",";
    out << "\"timestamp\":\""<<timestamp<<"\"";
    out << "}";

    return out.str();
}  

float Sample::getCo2(){
    return co2;
}

float Sample::getMetano(){
    return metano;
}

float Sample::getTemperatura(){
    return temperatura;
}

float Sample::getUmidade(){
    return umidade;
}