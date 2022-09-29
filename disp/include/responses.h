#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <WString.h>

class Response{

    protected:
        String msg;
        int code;
    
    public:
        Response(String msg, int code):msg(msg),code(code){}
    

    String getMsg(){
        return msg;
    }

    int getCode(){
        return code;
    }

};

class OkResponse : public Response{

    public:
        OkResponse():Response("Data sended",200){}
};

class ErrorResponse : public Response{

    public:
     ErrorResponse(int errorCode, String msg):Response(msg,errorCode){}

};

#endif