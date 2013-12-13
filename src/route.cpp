#include <node.h>
#include <string>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "handler_route.h"
#include "request.h"
#include "route.h"

using namespace v8;

namespace iroute{

  static int isAdd = 0;
	
  static handler_route **handler_p_get;           //指向get的handler实例指针数组的指针
  static int get_len;
  static handler_route **handler_p_post;          //post
  static int post_len;
  static handler_route **handler_p_put;           //put
  static int put_len;
  static handler_route **handler_p_delete;        //delete
  static int delete_len;

  static handler_route **handler_p_head;        //head
  static int head_len;
  static handler_route **handler_p_options;        //options
  static int options_len;
  static handler_route **handler_p_other;        //other
  static int other_len;

  static Persistent<Object> default_callback;     //默认回调函数，表示未匹配到控制器

}
//add param
/*
{
	get:[{uri:"/xxx/xxx",param:"uid&uid&aaa",callback:function}, {uri:"/xxx/xxx",param:"uid&uid&aaa",callback:function}],
	get_len:10,
	post:[...],
	post_len:10,
	put:[...],
	put_len:10,
	delete:[...],
	delete_len:10
}
*/
Handle<Value> route::add(const Arguments& args){
	HandleScope scope;

	if(iroute::isAdd){
		return ThrowException(Exception::TypeError(String::New("route::add can only execute once")));
	}	

	if(!args[0]->IsObject()){
		return ThrowException(Exception::TypeError(String::New("route::add first arg must be an object")));
	}
	if(!args[1]->IsFunction()){
		return ThrowException(Exception::TypeError(String::New("route::add second arg must be a function")));
	}


	Local<Object> add_obj = args[0]->ToObject();
	Local<Object> get_array = add_obj->Get(String::New("get"))->ToObject();
	iroute::get_len = add_obj->Get(String::New("get_len"))->Uint32Value();

	Local<Object> post_array = add_obj->Get(String::New("post"))->ToObject();
	iroute::post_len = add_obj->Get(String::New("post_len"))->Uint32Value();

	Local<Object> put_array = add_obj->Get(String::New("put"))->ToObject();
	iroute::put_len = add_obj->Get(String::New("put_len"))->Uint32Value();

	Local<Object> delete_array = add_obj->Get(String::New("delete"))->ToObject();
	iroute::delete_len = add_obj->Get(String::New("delete_len"))->Uint32Value();


	Local<Object> head_array = add_obj->Get(String::New("head"))->ToObject();
	iroute::head_len = add_obj->Get(String::New("head_len"))->Uint32Value();

	Local<Object> options_array = add_obj->Get(String::New("options"))->ToObject();
	iroute::options_len = add_obj->Get(String::New("options_len"))->Uint32Value();

	Local<Object> other_array = add_obj->Get(String::New("other"))->ToObject();
	iroute::other_len = add_obj->Get(String::New("other_len"))->Uint32Value();


	iroute::handler_p_get = new handler_route*[iroute::get_len]; //创建动态指针数组
	iroute::handler_p_post = new handler_route*[iroute::post_len];
	iroute::handler_p_put = new handler_route*[iroute::put_len];
	iroute::handler_p_delete = new handler_route*[iroute::delete_len];

	iroute::handler_p_head = new handler_route*[iroute::head_len];
	iroute::handler_p_options = new handler_route*[iroute::options_len];
	iroute::handler_p_other = new handler_route*[iroute::other_len];


	loop_add(get_array, iroute::handler_p_get, iroute::get_len);
	loop_add(post_array, iroute::handler_p_post, iroute::post_len);
	loop_add(put_array, iroute::handler_p_put, iroute::put_len);
	loop_add(delete_array, iroute::handler_p_delete, iroute::delete_len);

	loop_add(head_array, iroute::handler_p_head, iroute::head_len);
	loop_add(options_array, iroute::handler_p_options, iroute::options_len);
	loop_add(other_array, iroute::handler_p_other, iroute::other_len);

	iroute::default_callback = Persistent<Object>::New(args[1]->ToObject());

	iroute::isAdd = 1;
	return Undefined(); 
};


void route::loop_add(Handle<Object> array, handler_route **handler_p, int len){
	HandleScope scope;

	for(int i=0;i<len;i++){
		handler_route *hp = new handler_route(); //创建指向 handler_route 实例的指针
		Local<Object> obj = array->Get(i)->ToObject();
		

		std::string temp_uri = toCString(obj->Get(String::New("uri")));
		hp->char_uri = new char[temp_uri.length() + 1]; //获得uri的char*
		strcpy(hp->char_uri, temp_uri.c_str());
		hp->char_uri_len = strlen(hp->char_uri);//获得uri长度

		hp->callback = Persistent<Object>::New(obj->Get(String::New("callback"))->ToObject());
		hp->middle = Persistent<Object>::New(obj->Get(String::New("middle"))->ToObject());

		int param_array_length = Local<Array>::Cast(obj->Get(String::New("param")))->Length();//获得参数数组的长度
		hp->char_param = new char*[param_array_length];//生成指向指针数组的指针,保存参数数组
		hp->char_param_count = param_array_length;//保存参数数组长度

		for(int i=0;i<param_array_length;i++){

			String::Utf8Value value(obj->Get(String::New("param"))->ToObject()->Get(i)->ToObject());//conver to utf8-value

			std::string temp = *value;

			hp->char_param[i] = new char[temp.length() + 1];//将参数key存入内存
			strcpy(hp->char_param[i], temp.c_str());//复制key
			//std::cout<<hp->char_param[i]<<std::endl;
		}

		handler_p[i] = hp; //将指针存入 指针数组等待匹配

	}

};


Handle<Value> route::match(const Arguments& args){
	HandleScope scope;

	if(!iroute::isAdd){
		return ThrowException(Exception::TypeError(String::New("please run iroute.add(..) first!")));
	}
	Request req;

	String::Utf8Value url(args[0]->ToObject()->Get(String::New("url"))->ToString());//conver
	char *uri_c = *url;
	uri_c = tolower2(uri_c);
	req.url = uri_c;

	req.method = toCString(args[0]->ToObject()->Get(String::New("method")));
	
	worker_callback(req);



	int is_match = 0;

	if(req.handler_p){
		Persistent<Value> argv[4];
		argv[0] = Persistent<Value>::New(args[0]);
		argv[1] = Persistent<Value>::New(args[1]);
		argv[2] = req.handler_p->middle; //传入middle函数数组
		argv[3] = iroute::default_callback; //传入默认函数

		req.handler_p->callback->CallAsFunction(Object::New(), 4, argv);
		is_match = 1; 

		argv[0].Dispose();
		argv[1].Dispose();
	}
	else{
		Local<Value> argv2[2];
		argv2[0] = args[0];
		argv2[1] = args[1];

		iroute::default_callback->CallAsFunction(Object::New(), 2, argv2);

	}
	

	return Number::New(is_match); 
};



void route::worker_callback(Request &req){
	static const char sign1 = '?';

#ifdef WIN32
	char *char_uri = mystrsep(&req.url, &sign1);
#else
	char *char_uri = strsep(&req.url, &sign1);
#endif
	
	handler_route* handler_p = 0;
	std::string method = req.method;

	if(method == "GET"){ //匹配url
		handler_p = uri_match(iroute::handler_p_get, iroute::get_len, char_uri);
	}
	else if(method == "POST"){
		handler_p = uri_match(iroute::handler_p_post, iroute::post_len, char_uri);
	}
	else if(method == "PUT"){
		handler_p = uri_match(iroute::handler_p_put, iroute::put_len, char_uri);
	}
	else if(method == "DELETE"){
		handler_p = uri_match(iroute::handler_p_delete, iroute::delete_len, char_uri);
	}
	else if(method == "HEAD"){
		handler_p = uri_match(iroute::handler_p_head, iroute::head_len, char_uri);
	}	
	else if(method == "OPTIONS"){
		handler_p = uri_match(iroute::handler_p_options, iroute::options_len, char_uri);
	}
	else if(iroute::other_len){//如果不是以上6种，则进入other匹配
		handler_p = uri_match(iroute::handler_p_other, iroute::other_len, char_uri);
	}
	
	if(handler_p && handler_p->char_param_count){

		if(!req.url) handler_p = 0;
		else{	

			handler_p = param_match(handler_p, req.url) ? handler_p : 0;
		}
	}
	
	req.handler_p = handler_p;
	
};


handler_route* route::uri_match(handler_route **handler_p,int len,const char *char_uri){

	int char_uri_len = strlen(char_uri);
	int pos = 0;



	if(char_uri_len == 1 && char_uri[0] == '/'){
		char_uri = "/_iroute/_iroute/";
		char_uri_len = strlen(char_uri);	
	}

	if( char_uri[char_uri_len-1] != '/') pos = -1;



	for(int i=0;i<len;i++){

		int loc = strncmp(char_uri, handler_p[i]->char_uri, handler_p[i]->char_uri_len+pos);

		if( loc == 0 ){ //匹配uri地址成功

			return handler_p[i];
		}

	}

	return 0;
}

int route::param_match(handler_route *handler_p, char *param){
	

	static const char sign2 = '&';
	int has_match = 0;//已经匹配的参数
	int need_match = handler_p->char_param_count;//需要匹配的参数数量

#ifdef WIN32
	char *need_p = mystrsep(&param, &sign2);
#else
	char *need_p = strsep(&param, &sign2);
#endif
	
	while(need_p){
			for(int i=0;i<handler_p->char_param_count;i++){
				
					int key_len = strlen(handler_p->char_param[i]);
					int loc = strncmp(need_p, handler_p->char_param[i], key_len);

					if(loc == 0){//如果匹配到，则跳出循环
						has_match++;
						break;
					}
			}
			
		#ifdef WIN32
			need_p = mystrsep(&param, &sign2);
		#else
			need_p = strsep(&param, &sign2);
		#endif

	}

	return has_match == need_match;
}

std::string route::toCString(Handle<Value> strp){
      String::Utf8Value value(strp->ToString());//conver to utf8-value
      std::string str = *value;
      return str;
}




char* route::mystrsep(char** stringp, const char* delim)
{
  char* start = *stringp;
  char* p;

  p = (start != NULL) ? strpbrk(start, delim) : NULL;

  if (p == NULL)
  {
    *stringp = NULL;
  }
  else
  {
    *p = '\0';
    *stringp = p + 1;
  }

  return start;
}

char* route::tolower2(char* s)
{

	for(int i = 0; i<strlen(s); i++){
         s[i] = tolower(s[i]);
        
	}
  
  return s;
}