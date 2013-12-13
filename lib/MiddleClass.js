
function Middle(req, res, middle_func_array, default_callback){//middle类
	var me = arguments.callee;
	if(!(this instanceof me)) return new me(req, res, middle_func_array, default_callback);


	this.middle_func_array = middle_func_array;
	this.last_func_pos = middle_func_array.length - 1;
	this.now_func_pos = 0;
	this.default_callback = default_callback;
	this.req = req;
	this.res = res;
	this.init();
}
Middle.prototype = {
	init : function(){
		this.iroute_next();
	},
	iroute_next:function(err){
		var that = this;
		if(err){
			this.req.iroute_error = err; //保存iroute next的error
			return this.default_callback(this.req, this.res);//如果路由中有任何错误，则执行默认回调函数
		} 
		if(this.now_func_pos>this.last_func_pos){
			return;
		}
		else if(this.now_func_pos === this.last_func_pos){
			this.middle_func_array[this.now_func_pos](this.req, this.res);
		}
		else{
			this.middle_func_array[this.now_func_pos](this.req, this.res, function(err2){
				that.now_func_pos++;
				that.iroute_next(err2)}
			);
		}
		
	}

}

module.exports = Middle;