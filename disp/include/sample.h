#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <ctime>
#include <string>
#include <sstream>
using namespace std;

class Sample{
    private:
        float co2;
        float metano;
        float temperatura;
        float umidade;
        string timestamp;
    
    public:
        Sample(float co2,float metano, float temperatura,float umidade,string timestamp);
        string toJson();
        float getTemp();
        float getUmi();
        float getCo2();
        float getMet();
        string getTimestamp();

};

#endif