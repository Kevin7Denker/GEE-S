#ifndef _API_H_
#define _API_H_

#include <sample.h>
#include <HTTPClient.h>
#include <responses.h>

class API{

    private:
        String baseUrl;
        String endPoint;


    public:
        API(String baseUrl, String endPoint);
        Response send(Sample sample);

};

#endif