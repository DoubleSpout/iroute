#ifndef ROUTE_H
#define ROUTE_H
#include <node.h>
#include <string>
#include "handler_route.h"
#include "request.h"

using namespace v8;

class route {

 public:
  static Handle<Value> add(const Arguments& args);
  static Handle<Value> match(const Arguments& args);

  static std::string toCString(Handle<Value> strp);
  static void loop_add(Handle<Object> array, handler_route **handler_p,int len);
  static void worker_callback(Request &req);

  static handler_route* uri_match(handler_route **handler_p, int len,const char *char_uri);
  static int param_match(handler_route *handler_p, char *param);
  static char* mystrsep(char** stringp, const char* delim);
  static inline char* tolower2(char* s);
  route(){};
  ~route(){};


  
};

#endif