#ifndef REQUEST_H
#define REQUEST_H
#include <node.h>
#include <string>
#include "handler_route.h"

using namespace v8;

class Request {

 public:

    char *url;              //保存url
    std::string method;           //保存请求方法
    handler_route *handler_p;
    Request(){};
    ~Request(){};
  
};

#endif