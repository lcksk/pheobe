

var _Control = Class.extend({
	init : function() {
		console.log("_Control.init");
	},
});


var _K = Class.extend({
	websocket: null,
	eventListeners:null,
	control:null,
	init: function() {
		eventListeners = new Array();
		fireEvent =  function(e0) {
			for(var i = 0; i < eventListeners.length; i++) {
				eventListeners[i](e0);
			}
		};

		messageReceived = function(e0) {
			try {
				msg = JSON.parse(e0.data);
				fireEvent(msg);
			} catch(e) {
				console.log(e);
			}
		};
	},
	addEventListener: function(fn) {
		console.log("addeventListeners");
		eventListeners[eventListeners.length] = fn;
	},
	bind: function(uri) {
		console.log("bind");
		websocket = new WebSocket(uri);
		websocket.onopen = function(e0) {
			var event = {};
			event.event = "open";
			fireEvent(event);
		};
		websocket.onclose = function(e0) {
			var event = {};
			event.event = "close";
			fireEvent(event);
		};
		websocket.onmessage = function(e0) {
			messageReceived(e0);
		};
		websocket.onerror = function(e0) {
			var event = {};
			event.event = "error";
			fireEvent(event);
		};
	},
	subscribeEvent: function(name) {
		console.log("subscribeEvent: " + name);
		var object = {"jsonrpc": "2.0", "method": name, "params": 1, "id": 10}; // TODO: params isn't required.
		websocket.send(JSON.stringify(object));
	},
	unsubscribeEvent: function(name) {
		console.log("unsubscribeEvent: " + name);
		var object = {"jsonrpc": "2.0", "method": name, "params": 1, "id": 10}; // TODO: params isn't required.
		websocket.send(JSON.stringify(object));
	},
	addDmsEvent: function() { //@deprecataed, subscribeEvent is preferred instead
		console.log("addDmseventListeners");
		var object = {"jsonrpc": "2.0", "method": "dmc::addEventListener", "params": 1, "id": 10};
		websocket.send(JSON.stringify(object));
	},
	removeDmsEvent: function() {//@deprecataed, subscribeEvent is preferred
		console.log("removeEvent");
		var object = {"jsonrpc": "2.0", "method": "dmc::removeEventListener", "params": 1, "id": 10};
		websocket.send(JSON.stringify(object));
	},
	subscribeServer : function (uuid) {
		console.log("subscribeServer : " + uuid);
		var object = {"jsonrpc": "2.0", "method": "dmc::subscribeServer", "uuid": uuid, "id": 10};
		websocket.send(JSON.stringify(object));
	},
//	browseRootContainer: function () {
//		console.log("browseRootContainer");
//		var object = {"jsonrpc": "2.0", "method": "dmc::browseRootContainer", "id": 10};
//		websocket.send(JSON.stringify(object));
//	},
	browse: function (objectId, startIndex, count) {
		console.log("browse");
		var object = {"jsonrpc": "2.0", "method": "dmc::browse", "id": 10, "objectId": objectId, "startIndex": startIndex, "count" : count};
		websocket.send(JSON.stringify(object));
	},
	volume : function(up) { // TODO
		if(up == true) {
		}
		else {
		}
	},
	channel : function(up) { // TODO
		if(up == true) {
			
		}
		else  {
			
		}
	},
	dca : function(no) { // TODO : Direct Channel Acccess
		
	},
	uninit: function() {
		this.removeDmsEvent(); //TODO: It requires this. I need to get clear concept for this. dig into it.
		
		websocket.onclose = function(){};  // make it empty.
		websocket.close();
	},
	getWebSocketURL : function() {
		var pcol;
		var u = document.URL;
		if (u.substring(0, 5) == "https") {
			pcol = "wss://";
			u = u.substr(8);
		} else {
			pcol = "ws://";
			if (u.substring(0, 4) == "http")
				u = u.substr(7);
		}
		u = u.split('/');
		return pcol + u[0];
	},
	createControl: function() {
		control = new _Control();
	}
});



(function() {
	var K = window.K = new _K();
})();


