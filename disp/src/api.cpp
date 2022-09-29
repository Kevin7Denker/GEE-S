/************* Biblioteca *************/ 
#include "api.h"

/************* Parametros de Comunicação *************/
API::API(String baseUrl, String endPoint):baseUrl(baseUrl),endPoint(endPoint){}

/************* POST da Amostra *************/
Response API::send(Sample sample){
   
    try{
        HTTPClient http;
        WiFiClient client;
        http.begin(baseUrl+endPoint);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(String(sample.toJson().c_str()));
    
        if(httpResponseCode == 200){
            return OkResponse();
        }else{
            return ErrorResponse(httpResponseCode,http.getString());
        }
    
        http.end();
    }catch(...){
        return ErrorResponse(-1,"O servidor não está respondendo!!!");
    }
}