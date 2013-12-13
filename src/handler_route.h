#ifndef HANDLER_H
#define HANDLER_H
#include <node.h>
#include <string>


using namespace v8;

class handler_route {

 public:
  char *char_uri;
  int char_uri_len;
  
  char **char_param;
  int char_param_count;

  Persistent<Object> callback;   //js注册的回调函数
  Persistent<Object> middle;     //存放middle函数数组

  static int count;                       //此接口的访问次数
  
  handler_route(){};
  ~handler_route(){};
  
};

#endif