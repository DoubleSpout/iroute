
var route_cc = require('../build/Release/iroute.node');
var Middle = require('./MiddleClass.js');
var iroute = {}

var os = require("os");
var URLSTR_EX = "get:/user/aaa/?key1&key2&key3";
var get_ary = [];
var post_ary = [];
var put_ary = [];
var delete_ary = [];
var head_ary = [];
var options_ary = [];
var other_ary = [];

/*
urlstr example

iroute.add(
[
["get:/user/aaa/[ggg/aaaa]?key1&key2&key3", function middle1(req,res,iroute_next){
	iroute_next(1);
}, function middle2(req,res,iroute_next){
	iroute_next(1);
}, function(req,res){
	res.end("hello")
}],
...
])

get 表示方法
/user/aaa/[ggg/aaaa] 表示请求路径，中括号表示可选url路径
key1&key2&key3 表示此action必须接受的参数

*/
iroute.add = function(array, default_callback){
	var that = this;
	array.forEach(function(v,i){

			var urlstr = v[0];
			var temp = urlstr.split(':');

			//get method
			var method = temp[0];
			if(!method){
				throw('add error; '+urlstr+': must have methd, example is "'+ URLSTR_EX +'"')
			}
			method = method.toLowerCase();

			temp = temp[1].split('?') || false;

			//get uri
			var uri = temp[0].toLowerCase();
			if(!uri){
				throw('add error; '+urlstr+': must have url, example is "'+ URLSTR_EX +'"')
			}
			uri = uri.split('[')[0];
			
			if(uri === '/'){
				uri = '_iroute/_iroute/';
			}
			var last_po = uri.length-1;
			if(uri[last_po] !== '/'){

				uri += '/' 
			}
			if(uri[0] !== '/'){
				uri = '/'+uri;
			}

			//get param
			var key = temp[1] || '';
			if(key){
				key = key.split('&')
						 .filter(function(v,i){
						 	return v;
						 })
						 .map(function(v,i){
						 	if(v) v += '=';
						 	return v;
						 })					
			}

			var middle_func_array = []; //保存中间拦截器的方法
			var j=1;
			while(v[j]){
				if('function' !== typeof v[j]){
					break;
				}
				middle_func_array.push(v[j++]);
			}

			var router_obj = {
				uri:uri,
				param:key,
				middle:middle_func_array, //多存放一个middle数组
				callback:Middle //存放Middle类
			}

			switch (method){
				case "get":
					get_ary.push(router_obj);
					break;
				case "post":
					post_ary.push(router_obj);
					break;
				case "put":
					put_ary.push(router_obj);
					break;
				case "delete":
					delete_ary.push(router_obj);
					break;
				case "head":
					head_ary.push(router_obj);
					break;
				case "options":
					options_ary.push(router_obj);
					break;
				case "all":
					get_ary.push(router_obj);
					post_ary.push(router_obj);
					put_ary.push(router_obj);
					delete_ary.push(router_obj);
					head_ary.push(router_obj);
					options_ary.push(router_obj);
				case "other":
					other_ary.push(router_obj);
			}	

	});

	default_callback = default_callback || function(req,res){};

	var add_obj_cc = {
		"get" : get_ary,
		"get_len" : get_ary.length,
		"post" : post_ary,
		"post_len" : post_ary.length,
		"put" : put_ary,
		"put_len" : put_ary.length,
		"delete" : delete_ary,
		"delete_len" : delete_ary.length,	
		"head" : head_ary,
		"head_len" : head_ary.length,
		"options" : options_ary,
		"options_len" : options_ary.length,
		"other" : other_ary,
		"other_len" : other_ary.length,

	};

	route_cc.add(add_obj_cc, default_callback);

	return that;

}


iroute.route = route_cc.match;



module.exports.add = iroute.add;
module.exports.route = iroute.route;

module.exports.connect = function(array, default_callback){

	iroute.add(array, default_callback)

	return function(req,res,next){

		if(iroute.route(req,res) != 1) next();

	}
};