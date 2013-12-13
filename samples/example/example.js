var iroute = require('../index')


iroute.add([
["get:/ex1/",
function(req,res,ir_next){console.log('middle1'); req.ir_count = 0;req.ir_count++; setTimeout(function(){ir_next()},500); },
function(req,res,ir_next){console.log('middle2');req.ir_count++; ir_next();},
function(req,res,ir_next){console.log('middle3'); req.ir_count++;setTimeout(function(){ir_next()},2000); },
function(req,res,ir_next){console.log('middle4');req.ir_count++; ir_next()},
function(req,res){res.end('req.ir_count is:' + req.ir_count)}],
["get:/ex2/ex2/?key",function(req,res){res.end('ex2')}],
["post:/ex3/ex3/ex3/?key1&key2&key3",function(req,res){res.end('ex3')}],
["put:/ex4",function(req,res){res.end('ex4')}],
["delete:/ex5/?key1&key2&key3",function(req,res){res.end('ex5')}]
],function(req,res){
	res.statusCode = 404;
	res.end('404')
})


var http = require('http');

http.createServer(function (req, res) {

	var buf_list = [];
	var len=0;
	req.on("data",function(chunk){
		buf_list.push(chunk);
		len += chunk.length;
	})
	req.on("end", function(){
		req.body = Buffer.concat(buf_list, len).toString();
		if(req.url != '/favicon.ico') {
		 //route the request
		 iroute.route(req,res);
		} 
	})

}).listen(8124);

console.log('Server running at http://127.0.0.1:8124/');
















/*

function request(path, method, cb){

	var options = {
	  hostname: 'localhost',
	  port: 8124,
	  path: path,
	  method: method
	};
	var req = http.request(options, function(res) {

	  //console.log('STATUS: ' + res.statusCode);
	 //console.log('HEADERS: ' + JSON.stringify(res.headers));
	  res.setEncoding('utf8');
	  var s;
	  res.on('data', function (chunk) {
	  		s = chunk.toString();
	  });
	  res.on('end',function(){
	  	cb(null, res, s+'');
	  })
	});

	req.on('error', function(e) {
		console.log(e)
	  console.log('problem with request: ' + e.message);
	});

if(method === "POST" || method !== "PUT"){
	// write data to request body
	req.write('data\n');
	req.write('data\n');
}
	req.end('\n');

}

request('/ex1/', 'GET' ,function(err, res, s){
	console.log(s);
})

*/


