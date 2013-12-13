#include <node.h>
#include "route.h"



using namespace v8;

void Init(Handle<Object> target) {

  HandleScope scope;

  target->Set(String::NewSymbol("add"),
           FunctionTemplate::New(route::add)->GetFunction());

  target->Set(String::NewSymbol("match"),
           FunctionTemplate::New(route::match)->GetFunction());

}

NODE_MODULE(iroute, Init)