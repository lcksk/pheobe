	var windowManager;
    var menuActive = true;
	var socket;

    function get_appropriate_ws_uri()
    {
	    var pcol;
	    var u = document.URL;

		if(u.substring(0,7) == "file://") {
			return "ws://127.0.0.1";
		}
		
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
    }
	
	function openSocket() {
        var uri = get_appropriate_ws_uri() + "/BAM";
        var rpc = "rpc-bam";
        try{
            socket = new WebSocket(uri, rpc);
        } catch(exception) {
		alert("caught");
            socket = new MozWebSocket(uri, rpc);
        }
        socket.onopen = function(){
			alert("opened!");
			service();
			
		}
		socket.onclose = function() {
		//alert("closed!");
        }
        socket.onmessage = function(msg){
            var myObject = JSON.parse(msg.data);
			alert(msg.data);
         //   windowManager.menu.sequencer.call(myObject.id, myObject);
        }
	}
				function launch() {
	 socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IWindowManager::launch", "params":{ "serverPID" : "0", "applicationType" : "Webapp", "applicationURL" : "http://www.google.com", "applicationParameters" : "", "compositingParameters" : { "dimensions" : { "w" : 1280, "h" : 720 }, "visibility" : true, "opacity" : 255, "zorder" : 1, "location" : { "x" : 0, "y" : 0, "w" : 1280, "h" : 720}, "tunnelCapable" : false}, "giveFocus" : true, "enableDebug" : true }, "id": "1"}));
     }
	 				function launch2() {
	 socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IWindowManager::launch", "params":{ "serverPID" : "0", "applicationType" : "Webapp", "applicationURL" : "http://css-tricks.com/examples/WebKitScrollbars/", "applicationParameters" : "", "compositingParameters" : { "dimensions" : { "w" : 1280, "h" : 720 }, "visibility" : true, "opacity" : 255, "zorder" : 2, "location" : { "x" : 0, "y" : 360, "w" : 1280, "h" : 720 }, "tunnelCapable" : false}, "giveFocus" : true, "enableDebug" : true }, "id": "2"}));
     }
	 

function socketInit() {
		openSocket();
		//openAppSocket("1234");
	}
function getState() {
	alert("sending");
	socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IApplicationManager::getRuntimeStatistics", "params":{ "uid" : "12055" }, "id": "3"}));
	alert("sent");
}

function getRuntimeStatistics() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IApplicationManager::getRuntimeStatistics", "params":{ "uid" : "12055" }, "id": "3"}));
}
	

function window() {
	 socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IWindowManager::getWindowList", "params":{ "serverPID" : "0", "applicationType" : "Webapp", "applicationURL" : "http://www.google.com", "applicationParameters" : "", "compositingParameters" : { "dimensions" : { "w" : 1280, "h" : 720 }, "visibility" : true, "opacity" : 255, "zorder" : 1, "location" : { "x" : 0, "y" : 0, "w" : 1280, "h" : 720}, "tunnelCapable" : false}, "giveFocus" : true, "enableDebug" : true }, "id": "3"}));
	 }

function service() {

	  socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IServiceManager::getServiceList", "id": "3"}));
	 }
	
window.onload = socketInit;
