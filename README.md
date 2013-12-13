# iroute(fast and simple nodejs http/https router module) [![Build Status](https://travis-ci.org/DoubleSpout/iroute.png?branch=master)](https://travis-ci.org/DoubleSpout/iroute)

if you only want to build a simple http server and don't want to use express,so iroute module could simple and fast to handler the request to action.
the module are also be used in flat.js framework.

## Installing the module

With [npm](http://npmjs.org/):

iroute module is supported windows, linux, mac.

Make sure, node-gyp has installed.

     npm install iroute

From source:

     git clone https://github.com/DoubleSpout/iroute.git
     cd iroute
     node-gyp rebuild

To include the module in your project:

     var iroute = require('iroute');

##benchmark

to run 1000 handler and match 10000 times,regexp match and iroute result is follow:

      regexp: 1007ms
      cb1 run times :10000
      iroute: 251ms
      cb2 run times :10000

wow!!it's more than 3 times faster than regex match router.

## example

    var iroute = require('iroute')

    iroute.add([

      ["get:/hello/world",function(req,res){res.end('hello world')}],

    ],function(req,res){

        res.statusCode = 404;

        res.end('404')

    })

    var http = require('http');

    http.createServer(function (req, res) {

      iroute.route(req,res);

    }).listen(8124);

then request the 127.0.0.1:8124 you can see 'hello world'

##API

iroute.add(routearray [,not_match_function]);

iroute.route(req,res); 
if iroute.route match the request,it will return 1,else return 0;

routearray:

  [
    [{method}:{url}?{key1}&{key2}, middle_function1, middle_function2, .., handle_function],
    ...
  ]

  example:

      [

        ["get:/hello/world/?key1&key2", function middle1(req,res,ir_next){
          req.ir_count = 0;
          req.ir_count++;
          ir_next()},
        function middle2(req,res,ir_next){ req.ir_count++; process.nextTick(function(){ ir_next() }) },
        function middle3(req,res,ir_next){ req.ir_count++; setTimeout(function(){ ir_next(); },2000) }, //Asynchronous is ok
        function middle4(req,res,ir_next){ req.ir_count++; ir_next()},
        function handle(req,res){
          res.end( req.ir_count+''); //  req.ir_count will be 4!
        }]

      ]

ir_next([error]):
if you don't call the function ir_next(),then will hang up the request,never responsed the rquest.so if you want to interrupt the routing, run the res.end() and never call the ir_next() or call the ir_next("some error")  handle the request to the not_match_function;
ir_next() function has an optional param error,if ir_next("some error");then will break the routing and call the not_match_function .it then setting the req.iroute_error = "some error";

not_match_function:
if iroute not match the request,the not_match_function will be called.It has two parameters,req and res.

support: get,post,put,delete,options,head,other (other include copy,trace,lock.. etc)

more example see /example/example.js

##expressjs example

      var express = require('express');
      var app = express();
      var iroute = require("iroute");

      var route_array = [
        ["get:/hello/world",function(req,res){res.end('hello world')}],
      ]

      app.use(iroute.connect(route_array));

      app.listen(3000);


##benchmark to express and express+iroute

      express only(1000 routes)
      ab -c 100 -n 20000 http://192.168.28.5:8127/test/
      Requests per second:    1271.83 [#/sec] (mean)

      ab -c 500 -n 20000 http://192.168.28.5:8127/test/
      Requests per second:    1237.43 [#/sec] (mean)

      ab -c 800 -n 20000 http://192.168.28.5:8127/test/
      Requests per second:    1191.17 [#/sec] (mean)



      express+iroute (1000 routes)
      ab -c 100 -n 20000 http://192.168.28.5:8128/test/
      Requests per second:    1886.01 [#/sec] (mean)

      ab -c 500 -n 20000 http://192.168.28.5:8128/test/
      Requests per second:    1773.27 [#/sec] (mean)

      ab -c 800 -n 20000 http://192.168.28.5:8128/test/
      Requests per second:    1829.89 [#/sec] (mean)
