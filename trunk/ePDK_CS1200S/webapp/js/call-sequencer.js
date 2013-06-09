/*While the function can be saved by just using:
	this.func = obj.func;
 this will save just a reference to the function with no access to the parent object.
 If parent object fields need to be changed or parameters need to be saved/passed, these values 
 need to be saved before sending off the call.
 */


function JSONCallback() {
	this.object = null;
	this.func = null;
	this.params = null;
}

var callSequencer = Class.extend({
    _list: [],
    init: function () {
		
    },    
    push: function (id, object, func, params) {
	var index = parseInt(id);
	var callback = new JSONCallback();
	callback.object = object;
	callback.func = func;
	callback.params = params;
	this._list[index] = callback;
    },
	
    call: function (id, myObject) {
	//alert(id);
	index = parseInt(id);
	var cb = this._list[index];
	//alert("calling " + index);
	cb.func(cb.object, cb.params, myObject);
	//alert("return " + index);
	//this._list.splice(index, 1);
    }

});



