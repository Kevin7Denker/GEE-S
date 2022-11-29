#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <ctime>
#include <Arduino.h>
#include <sstream>
using namespace std;

class Sample{
    private:
        float co2;
        float metano;
        float temperatura;
        float umidade;
        String timestamp;
    
    public:
        Sample(float co2,float metano, float temperatura,float umidade,String timestamp);
        String toJson();
        float getTemp();
        float getUmi();
        float getCo2();
        float getMet();
        String getTimestamp();

};

#endif