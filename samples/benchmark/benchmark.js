/*
传统的路由正则匹配
摘自expressjs
*/

var n1,n2,l1,l2
n1 = n2 = 100;//route_times 注册路由个数，统一为100个
l1 = l2 = 1000;//match_times 匹配路由次数，统一为1000次
var req = {method:"GET",url:"/test1/test2/test3/?id=1"}
var res = {end:function(){}}
var iroute = require('../index.js')

var cb1_run = 0;
var cb2_run = 0;
var cb1 = function(){
	cb1_run++
	//console.log(11)
}
var cb2 = function(){
	cb2_run++
	//console.log(22)
}




/*begin test regexp match*/
var rout_ary = [];
for(var i=0;i<n1-1;i++){
	var s = "/^\/test1\/test2\/test3\/"+n1+"\/(?:([^\/]+?))\/?$/i";
	rout_ary[i]={};
	rout_ary[i].exp = new RegExp(s)
	rout_ary[i].fn = cb1;
}

rout_ary[n1-1]={}
rout_ary[n1-1].exp = /^\/test1\/test2\/test3\/(?:([^\/]+?))\/?$/i;
rout_ary[n1-1].fn = cb1;


console.time("regexp")
while(l1--){
	for(var j=0;j<n1;j++){
		if(rout_ary[j].exp.test(req.url)){
			rout_ary[j].fn();
			break;
		}
	}	
}
console.timeEnd("regexp")
console.log("cb1 run times :"+cb1_run)





/*begin test iroute match*/
var irtoue_array= [];
for(var i=0;i<n2-1;i++){
	irtoue_array.push(["get:/test1/test2/test3"+i+"/?id",cb2]);
}
irtoue_array.push(["get:/test1/test2/test3/?id",cb2])



iroute.add(irtoue_array);
console.time("iroute")
while(l2--){
	iroute.route(req,res)	
}
console.timeEnd("iroute")
console.log("cb2 run times :"+cb2_run)