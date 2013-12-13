var iroute = require('../index')


iroute.add([
["get:/test1/",
function(req,res,ir_next){
	req.ir_count = 0;
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	//setTimeout(function(){
		ir_next();
	//},500)
	
},
function(req,res,ir_next){
	req.ir_count++;
	setTimeout(function(){
		ir_next();
	},100)
	
},
function(req,res,ir_next){
	req.ir_count++;
	//process.nextTick(function(){
		ir_next();
	//})	
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
 function(req,res){res.end(req.ir_count+'')}],
["get:/test2/test2/test2/test2/",function(req,res){res.end('test2')}],
["get:/test3/test3/test3/test3?key1&key2&key3",function(req,res){res.end('test3')}],

["post:/test4",function(req,res){res.end('test4')}],
["post:/test5/test5/test5/test5",function(req,res){res.end('test5')}],
["post:/test6/test6/test6/test6?key1&key2&key3",
function(req,res,ir_next){
	req.ir_count = 0;
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res){res.end(req.ir_count+'')}],
["post:/test7/test7/test7/test7?key1&key2&key3",
function(req,res,ir_next){
	req.ir_count = 0;
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	setTimeout(function(){

		ir_next();
	},2000)
	
},
function(req,res,ir_next){
	req.ir_count++;
	res.end(req.ir_count+'');
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res){res.end(req.ir_count+'')}],
["post:/test8/test8/test8/test8?key1&key2&key3",
function(req,res,ir_next){
	req.ir_count = 0;
	req.ir_count++;
	ir_next();
},
function(req,res,ir_next){
	req.ir_count++;
	setTimeout(function(){

		ir_next(1);
	},2000)
	
},
function(req,res,ir_next){
	req.ir_count++;
	res.end(req.ir_count+'');
},
function(req,res,ir_next){
	req.ir_count++;
	ir_next();
},
function(req,res){res.end(req.ir_count+'')}],
["put:/test7",function(req,res){res.end('test7')}],
["put:/test8/test8/test8/test8",function(req,res){res.end('test8')}],
["put:/test9/test9/test9/test9?key1",function(req,res){res.end('test9')}],
["delete:/test10",function(req,res){res.end('test10')}],
["delete:/test11/test11/test11/test11",function(req,res){res.end('test11')}],
["delete:/test12/test12/test12/test12?key1&key2&key3&key4&key5&key6&key7&key8&key9&key10",function(req,res){res.end('test12')}],

["head:/test6/test6/test6/test6?key1&key2&key3",function(req,res){res.end('')}],
["options:/test6/test6/test6/test6?key1&key2&key3",function(req,res){res.end('test14')}],
["other:/test6/test6/test6/test6?key1&key2&key3",function(req,res){res.end('test15')}],
["get:/",function(req,res){res.end('test0')}]

],function(req,res){
	res.statusCode = 404;
	res.end('404')
})





var http = require('http');

var assert = require('assert');


http.createServer(function (req, res) {
	//request.url
	//request.method
	if(req.method == "HEAD" ||req.method == "GET" ){
		return iroute.route(req,res);
	}
	var buf_list = [];
	var len=0;
	req.on("data",function(chunk){
		buf_list.push(chunk);
		len += chunk.length;
	})
	req.on("end", function(){
		req.body = Buffer.concat(buf_list, len).toString();
		if(req.url != '/favicon.ico') {

		 iroute.route(req,res);

		} 
	})


}).listen(8124);

console.log('Server running at http://127.0.0.1:8124/');





var test_num = 24
var test_back = function(msg){
	console.log(msg, 'test ok')
	if(!--test_num){
		process.exit(0)
	}

}

http.globalAgent.maxSockets = 40;

function request(path, method, cb){


if(method == 'get' || method == 'GET'){

	http.get("http://127.0.0.1:8124"+path, function(res) {
	  //console.log("Got response: " + res.statusCode);

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

		}).on('error', function(e) {
		  console.log("Got error: " + e.message);
		});
}
else{
				var options = {
				  hostname: '127.0.0.1',
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
				  console.log('problem with request: ' + e.message);
				});

			if(method === "POST" || method !== "PUT"){
				// write data to request body
				req.write('data\n');
				req.write('data\n');
			}
				req.end();
}


}




//begin test
//get
setTimeout(function(){


request('/', 'GET', function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test0')
	test_back('get0')
})

request('/TEST1/', 'get', function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'8')
	test_back('get1_middle_sucess')
})

request('/TEST1', 'GET', function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'8')
	test_back('get2_middle_sucess')
})

request('/test2/test2/test2/test2/','GET', function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test2')
	test_back('get3')
})



	request('/test3/test3/test3/test3?key1=%92%e5%b1%b1%e4%b8%9c%e7%9c%81&key2=%92%e5%b1%b1%e4%b8%9c%e7%9c%81&key3=%92%e5%b1%b1%e4%b8%9c%e7%9c%81', 'GET', function (error, response, body) {
		assert.equal(response.statusCode,200)
		assert.equal(body,'test3')
		test_back('get4')
	})

	request('/test3/test3/test3/test3/?key1&key2&&&key3=123', 'GET', function (error, response, body) {
		assert.equal(response.statusCode,404)
		assert.equal(body,'404')
		test_back('get5')
	})


	request('/test3/test3/test3/test3?key1=&key2=&key3=', 'GET', function (error, response, body) {
		assert.equal(response.statusCode,200)
		assert.equal(body,'test3')
		test_back('get6')
	})

	request('/test3/test3/test3/test3/?key1=1111&key2=22222&key3=333','GET',  function (error, response, body) {
		assert.equal(response.statusCode,200)
		assert.equal(body,'test3')
		test_back('get7')
	})



//post
request('/test4','POST',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test4')
	test_back('post1')
})

request('/test5/test5/test5/test5/','POST',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test5')
	test_back('post2')
})

request('/test6/test6/test6/test6?key1=1&key2=2&key3=3','POST',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'4')
	test_back('post3_middle_4')
})

request('/test7/test7/test7/test7?key1=1&key2=2&key3=3','POST',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'3')
	test_back('post3_middle_ircoutz_3')
})


request('/test8/test8/test8/test8?key1=1&key2=2&key3=3','POST',  function (error, response, body) {
	assert.equal(response.statusCode,404)
	assert.equal(body,'404')
	test_back('post3_middle_ircoutz_404')
})

//put

request('/test7','PUT',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test7')
	test_back('put1')
})

request('/test8/test8/test8/test8/','PUT',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test8')
	test_back('put2')
})

request('/test9/test9/test9/test9/?key1=111','PUT',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test9')
	test_back('put3')
})




//delete

request('/test10','DELETE',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test10')
	test_back('del1')
})

request('/test11/test11/test11/test11','DELETE',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test11')
	test_back('del2')
})

request('/test12/test12/test12/test12?key1=&key2=&key3=&key4=&key5=&key6=&key7=&key8=&key9=&key10=0','DELETE',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test12')
	test_back('del3')
})







//delete

request('/test6/test6/test6/test6?key1=1&key2=2&key3=3','head',  function (error, response, body) {
	assert.equal(response.statusCode,200)

	test_back('HEAD')
})


request('/test6/test6/test6/test6?key1=1&key2=2&key3=3','options',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test14')
	test_back('OPTIONS')
})



//delete

request('/test6/test6/test6/test6?key1=1&key2=2&key3=3','trace',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test15')
	test_back('TRACE')
})

request('/test6/test6/test6/test6?key1=1&key2=2&key3=3','copy',  function (error, response, body) {
	assert.equal(response.statusCode,200)
	assert.equal(body,'test15')
	test_back('COPY')
})

request('/test6/test6/test6/?key1=1&key2=2&key3=3','lock',  function (error, response, body) {
	assert.equal(response.statusCode,404)
	assert.equal(body,'404')
	test_back('LOCK')
})



},1000)


