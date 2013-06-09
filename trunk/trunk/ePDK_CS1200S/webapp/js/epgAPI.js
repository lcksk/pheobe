// JavaScript Document

		var choice;
		var socket;
		var jsonString;
		var mpJsonStr;
		var aID;
		var context;
		var MPsocket;
		var myObject;
		var myObjCopy;
		var startDate;
		var endDate;
		var dateID;
		var dur;
		var timeOut;
		var ccURL;
		var ccTimeout;
		var ccTest = false;
		var ccDuration;
		var ccLastID;
		var ccID;
		var numCC;
		var firstIndex;
		var continueScan = false;
		//var tempStorage = getSessionStorage() || dispError('Session Storage not supported.');
		var datesArr = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sept', 'Oct', 'Nov', 'Dec'];
		var serviceList = new Array();
		var uriList = new Array();
		var startVal = 0;
		var outputFormat = 'artemisxml';
		
/*************************************************************** Media Player *********************************************************************************************************/ 		
		
		function setMP(dvb){
			switch(choice){
				case "setURI":
				{
					mpJsonStr = JSON.stringify({"jsonrpc": "2.0", 
						"method": "IMediaPlayer::setDataSource", 
						"params" : {"dataSource" : dvb
									},					  
						"id": "11"}
					);
					choice = "";					
					break;
				}
				case "prepare":{
					mpJsonStr = JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::prepare", "id": "12"});
					choice = "";
					break;
				}
				case "start":{
					mpJsonStr = JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::start", "id": "13"});
					choice = "";
					break;
				}
				default:{
					mpJsonStr = "";
					choice = "";
					break;
				}
			}
		}
		
		function openMPSocket(id) {
			var dvb = uriList[id];
			var uri;
			var rpc = "rpc-mediaplayer";
			if(MPsocket == undefined){
				uri = get_appropriate_ws_uri();				
				uri = uri + "/MediaPlayer";
				try{
					MPsocket = new WebSocket(uri, rpc);
				} catch(exception) {
					MPsocket = new MozWebSocket(uri, rpc);
				}
			}
			else
			{
				choice = "setURI";
				setMP(dvb);
				MPsocket.send(mpJsonStr);
				$('#epgRqResult').append("<p>Sent: <xmp>"+mpJsonStr+'</xmp></p>');
			}
			
			MPsocket.onopen = function() {
				choice = "setURI";
				setMP(dvb);
				MPsocket.send(mpJsonStr);
				$('#epgRqResult').append("<p>Sent: <xmp>"+mpJsonStr+'</xmp></p>');				
			}
			
			MPsocket.onclose = function() {
                //do nothing
            }
			
			MPsocket.onmessage = function(msg){
				$('#epgRqResult').append('<p>Received: <xmp>'+msg.data+'</xmp></p>');
				var myObject = JSON.parse(msg.data);
				switch (myObject.id) {
					case "11":	// setDataSource
						choice = "prepare";
						setMP(dvb);
						break;
					case "12":	// "IMediaPlayer::onPrepared"
						choice = "start";
						var resultId = '#result' + ccID;
						
						$(resultId).focus().html(myObject.error).css('color', 'green');
						numCC++;
						$("#numChanges").html('Number of changes: ' + numCC);
						setMP(dvb);
						break;
					case "13":	// "IMediaPlayer::start"
						//alert("channel changed");
						if(!!ccTest)
						{
							var currentTime = new Date();
							if(currentTime.getTime() < dur)
							{
								setTimeout("testStart();", (parseInt(timeOut)));
							}
							else{
								ccTest = false;
								$("#changeChannel-form").dialog("close");
								$("#resultsDiv").html("<h1>Number of Successful Channel Changes: "+ numCC + "</h1>");
							}
						}
						break;
					default:{
						mpJsonStr = "";
						break;
						}
				}
				if(mpJsonStr != ""){
					$('#epgRqResult').append("<p>Sent: <xmp>"+mpJsonStr+'</xmp></p>');
					//setTimeout("MPsocket.send", 1, mpJsonStr);
					MPsocket.send(mpJsonStr);
					mpJsonStr = "";
					
				}
			}
		}
/******************************************************************************************************************************************/
		
		function setJSON(aID){
			switch(choice){
				case "sl": //Service List, Limit 256
				{
					jsonString = JSON.stringify({ "jsonrpc": "2.0", 
					"method": "ITVService::serviceListGet", 
					"params": { 
								"context" : context,
								"payload" : { "limit" : 256  } 
							  },
					 "id": "3"
					 });
					break;
				}
				case "nAndN": //Now and Next
				{
					jsonString = JSON.stringify({ 
					"jsonrpc": "2.0",
					"method": "ITVService::nowNextGet",
					"params": { 
								"context" : context,
								"payload" : {
											 "id" : aID ? aID:1075 ,
											 "outputFormat" : outputFormat//"artemisxml"
											} 
							  },
					"id": "3"
					});
					break;
				}
				case "slN10":{ //Get next limit (10) of service list and seek to position of last read
					jsonString = JSON.stringify({ 
					 "jsonrpc": "2.0",
					 "method": "ITVService::serviceListRead",
					 "params": { 
					 			"context" : context, 
								"payload" : { "limit" : 10  } 
							   }, "id": "3"
					});
					break;	
				}
				case "epgXML":{ //get guide data that matches query expression in XML
					var dateNow = new Date();
					/*returns tick since epoch in milliseconds so divide by 1000 for seconds*/
					//var ticksNow = (dateNow.getTime() / 1000);
					dateNow.setHours(23);
					dateNow.setMinutes(59);
					var ticksTill = (dateNow.getTime() / 1000);
							jsonString = JSON.stringify({ 
							"jsonrpc": "2.0",
							"method": "ITVGuide::query",
							"params": { 
										  "context" : context,
										  "payload" : {
														  "expression":"ServiceId == " + (aID ? aID:1075) +' && Start < ' + parseInt(ticksTill),
														  "expressionFormat":"artemis", 
														  "outputFormat" : outputFormat
													  } 
									 },
							"id": "3"
						});
					break;
				}
				case "epgJSON":{ //get guide data that matches query expression in JSON
				
					var dateNow = new Date();
					/*returns tick since epoch in milliseconds so divide by 1000 for seconds*/
					var ticksNow = (dateNow.getTime() / 1000);
					dateNow.setHours(23);
					dateNow.setMinutes(59);
					var ticksTill = (dateNow.getTime() / 1000);
					jsonString = JSON.stringify({ 
							"jsonrpc": "2.0",
							"method": "ITVGuide::query",
							"params": { 
										  "context" : context,
										  "payload" : {
														  "expression":"ServiceId == " + (aID ? aID:1075) +' && Start < ' + parseInt(ticksTill),
														  "expressionFormat":"artemis", 
														  "outputFormat" : outputFormat
													  } 
									 },
							"id": "3"
						});
					break;
				}
				case 'nDays':{
				
					jsonString = JSON.stringify({ 
							"jsonrpc": "2.0",
							"method": "ITVGuide::query",
							"params": { 
										  "context" : context,
										  "payload" : {
														  "expression":"ServiceId == " + (aID ? aID:1075) +
														  ' && Start <= ' + parseInt(endDate.getTime() / 1000) +
														  ' && Start >= ' + parseInt(startDate.getTime() / 1000),
														  "expressionFormat":"artemis", 
														  "outputFormat" : outputFormat
													  } 
									 },
							"id": "3"
						});
					break;
				}
				case "deleteSL":{
					jsonString = JSON.stringify({
						"jsonrpc": "2.0",
						"method": "ITVService::serviceListDeleteAll",
						"params": {
							"context" : context,
							"payload" : {
				
							}
						},
						"id": "90"
					});
					break;
				}
				case "blindScan":{
					jsonString = JSON.stringify({
						"jsonrpc": "2.0",
						"method": "ITVConfig::scanStart",          /*scanBlindStart"*/
						"params": {
							"context" : context,
							"payload" : {
								"sourceId":0//,
								//"useNIT": 0
							}
						},
						"id": "91"
					});
					continueScan = true;
					break;
				}
				case "infoGet":{
					jsonString = JSON.stringify({
						"jsonrpc": "2.0",
						"method": "ITVConfig::scanInfoGet",
						"params": {
							"context" : context,
							"payload" : {
								"sourceId":0
							}
						},
						"id": "92"
					});
					break;
				}
				case 'seek':{
					jsonString = JSON.stringify({
						 "jsonrpc": "2.0",
						 "method": "ITVService::serviceListSeek",
						 "params": { 
									"context" : context, 
									"payload" : { 
												"offset" : 0,
												"whence": 0 
												} 
									},
						 "id": "2"
					});
					break;	 
				}
				case 'alt':{
					jsonString = "";
					break;
				}
				default:
				{
					jsonString = JSON.stringify({ "jsonrpc": "2.0", "method": "ITVService::serviceListGet", "params": { "context" : context, "payload" : { "limit" : 256  } }, "id": "3"});
					break;
				}
			}
		}


		  
  		/* Fetch data from socket and parse for display */
        function getEPG(aID) {
            var uri = get_appropriate_ws_uri();
            uri = uri + "/TV";
            var rpc = "rpc-tv";
            
            if(socket === undefined){
				//grab a new socket
	            try{
	                socket = new WebSocket(uri, rpc);
	            } catch(exception) {
	                socket = new MozWebSocket(uri, rpc);
	            }
            }else{
            	if((context == "undefined") || (context == null))
				{
					//we only need one context per session
                	jsonString = JSON.stringify({ "jsonrpc": "2.0", "method": "ITVService::createContext", "id": "1"});
				}
				else
				{
					//if context is set. select JSON string to send
					setJSON(aID);
				}
				if(jsonString != ""){
					try{
						socket.send(jsonString);
					}catch(e){
						console.log(e.msg);
					}
					//display what we sent for debug
               		$("#epgRqResult").append("Sent: "+jsonString+'</p>');
               		jsonString = "";
               }
            }
            socket.onopen = function(){
				if((context == "undefined") || (context == null))
				{
					//we only need one context per session
                	jsonString = JSON.stringify({ "jsonrpc": "2.0", "method": "ITVService::createContext", "id": "1"});
				}
				else
				{
					//if context is set. select JSON string to send
					setJSON(aID);
				}
				socket.send(jsonString);
				//display what we sent for debug
               	$("#epgRqResult").append("Sent: "+jsonString+'</p>');
               	jsonString = "";
            }
            
            socket.onclose = function() {
                //do nothing
            }
            
            socket.onmessage = function(msg){
                myObject = JSON.parse(msg.data);
                jsonString = "";
                
                $('#epgRqResult').append('<p>Received: <xmp>'+msg.data+'</xmp></p>');
                
                switch(myObject.id){
                	case '1':{
					//we have our context, now we set the Service list
                    context = myObject.result.value;
                    jsonString = JSON.stringify({ "jsonrpc": "2.0", "method": "ITVService::serviceListSet", "params": { "context" : context, "payload" : { "base" : "ignored" } }, "id": "2"});
                    break;
                	}
                	case '2': {
						//get appropriate Json string for choice if choice not = seek
						if(choice != 'seek'){
							setJSON(aID);
						}
						break;
	                }
	                case '90':{
						var tmp = myObject;
						choice = "blindScan";
						getEPG();
						break;
					}
					case '91':{
						choice = "infoGet";
						getEPG();
						break;
					}
					case '92':{
						if(myObject.result.busy){
							if(continueScan){
								setTimeout(function(){
									choice = "infoGet";
									getEPG();
								}, 1000);
							}
							$("#progressbar").progressbar({
								value : myObject.result.progress
							});
							$("#searchInfoBox span#totalServices").html(myObject.result.services);
							$("#searchInfoBox span#tvServices").html(myObject.result.TVServices);
							$("#searchInfoBox span#radioServices").html(myObject.result.radioServices);
							$("#searchInfoBox span#dataServices").html(myObject.result.radioServices);
							$("#searchInfoBox span#transponders").html(myObject.result.transponders);
						}else{
							$("#progressbar").progressbar({
								value : myObject.result.progress
							});
							continueScan = false;
						}					
						break;
					}
	                case'3': {
                    /*********************************************************************
					** parse recieved information
					*********************************************************************/
					switch(choice){
						case 'nAndN':{ //Now and Next
							if(outputFormat ==='artemisjson'){
								var tmp = myObject.result[1].substring(0, myObject.result[1].length -5);
								tmp += "}}\n";
								myObject.result[1]=tmp;	
								for( var i = 0; i < myObject.result.length; i++)
								{
									myObject.result[i] = JSON.parse(myObject.result[i]);	
								}
							}else{
								for(var i = 0; i < myObject.result.length; i++)
								{
									var xmlForParser = myObject.result[i] 
									var programList=xml2json.parser(xmlForParser, "", "compact");
									programList = programList.replace(/\'/gi, "\"");
									myObject.result[i] = JSON.parse(programList);
								}
							}
							break;
						}
						case 'epgJSON':{ //Guide data for Artemis ID
							if(outputFormat ==='artemisjson'){	
								for( var i = 0; i < myObject.result.length; i++)
								{
									myObject.result[i] = JSON.parse(myObject.result[i]);	
								}
								choice ='epgJSON';
							}else{
								for(var i = 0; i < myObject.result.length; i++)
								{
									var xmlForParser = myObject.result[i] 
									var programList=xml2json.parser(xmlForParser, "", "compact");
									programList = programList.replace(/\'/gi, "\"");
									myObject.result[i] = JSON.parse(programList);
								}
								choice = 'epgXML'
							}
							break;
						}
						case 'epgXML':{
							/*********************************************************
							** myObject.result contains an array of JSON strings.  
							** Convert all then store in same place within object 
							*********************************************************/
							if(outputFormat === 'artemisxml'){
								for(var i = 0; i < myObject.result.length; i++)
								{
									var xmlForParser = myObject.result[i] 
									var programList=xml2json.parser(xmlForParser, "", "compact");
									programList = programList.replace(/\'/gi, "\"");
									myObject.result[i] = JSON.parse(programList);
								}
								choice = 'epgXML'
							}else{
								for( var i = 0; i < myObject.result.length; i++)
								{
									myObject.result[i] = JSON.parse(myObject.result[i]);	
								}
								choice ='epgJSON';
							}
							break;
						}
						case 'slN10':{ //Service List Next 10
							// if at the end of the service list,seek to beginning of list.
							if(myObject.result.length == 0)
							{
								jsonString = JSON.stringify({
								 "jsonrpc": "2.0",
								 "method": "ITVService::serviceListSeek",
								 "params": { 
								 			"context" : context, 
											"payload" : { 
														"offset" : 0,
														"whence": 0 
														} 
											},
								 "id": "2"});
								 choice = 'alt';
							}
							break;
						}
						case 'alt':{
							break;
						}
						case 'seek':{
							jsonString = JSON.stringify({
								 "jsonrpc": "2.0",
								 "method": "ITVService::serviceListSeek",
								 "params": { 
								 			"context" : context, 
											"payload" : { 
														"offset" : 0,
														"whence": 0 
														} 
											},
								 "id": "2"
							});
							break;	 
						}
						case 'nDays':{
							choice = 'epgJSON';
							for( var i = 0; i < myObject.result.length; i++)
							{
								myObject.result[i] = JSON.parse(myObject.result[i]);								
							}
							break;
						}
						default:{
							// if at the end of the service list,seek to beginning of list.
							if(myObject.result.length == 0)
							{
								jsonString = JSON.stringify({
								 "jsonrpc": "2.0",
								 "method": "ITVService::serviceListSeek",
								 "params": { 
								 			"context" : context, 
											"payload" : { 
														"offset" : 0,
														"whence": 0 
														} 
											},
								 "id": "2"});
							}
							break;
						}
					}break;
                }
                }
                
                if (jsonString != "") {
					//Send string built in setJSON(aID);
                	socket.send(jsonString);
                    $('#epgRqResult').append("Sent: "+jsonString+'</p>');
                    jsonString = "";
                }
                if(choice != "infoGet"){
                	dispResults();
                }
            }
        }
		
		function get_appropriate_ws_uri(){
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
        }
		
		function outputChange(type){
			switch(type){
				case 'json':{
					outputFormat = 'artemisjson';
					break;
				}
				case 'xml':{
					outputFormat = 'artemisxml';
					break;
				}
				default:{
					break;
				}
			}
		}
		
        // How many rows do we have?
        function countRows() {
			var len;
			if((!! myObject) && (myObject.result != undefined)){
				len = myObject.result.length;
			}
			else{
				len = 0;
			}
			element('rowCount').innerHTML = len;
		}

        // Create the Now & Next and Full Guide buttons for a row
        function rowButtons( id, dvbURI ) {
            return '<form><input type="button" value="Get Now &amp; Next" onclick="javascript:nAndNGo(' + id + ')"/>' +
                '<input type="button" value="Full Day" onClick="javascript:fullDayGo(' + id + ')"/>' + '<br/>' +
				'<input type="button" value="Set Days" id="epgDates" onClick="javascript:setDates(' + id + ')"/>' +
				'<input type="button" value="Change Channel" id="mpSetURI" onClick="javascript:openMPSocket(' + id + ')"/></form>';
        }
		
        // Main display function
        function dispResults() {
            if(errorMessage) {
                element('resultsDiv').innerHTML = errorMessage;
                return;
            }
			
            countRows();    // update the row count each time the display is refreshed
			
			var smins, emins;
			var bwt = new bwTable();
			if(myObject != undefined && myObject != 'undefined'){
					var len = myObject.result.length;
			}
			else{
				var len = 0;
			}
			
			if(!!myObject) {	
				switch(choice){
					case 'nAndN':{
						bwt.setHeader(['Program Title', 'Description', 'Start', 'End', 'Service ID','']);
						for(var i = 0; i < len; i++)
						{
							if(outputFormat ==='artemisjson'){	
								start = new Date((parseInt(myObject.result[i].progNowNext.Start) * 1000));
								end = new Date((parseInt(myObject.result[i].progNowNext.End) * 1000));
							}else{
								start = new Date((parseInt(myObject.result[i].prognownext.start) * 1000));
								end = new Date((parseInt(myObject.result[i].prognownext.end) * 1000));
							}
							if(start.getMinutes() < 10)
							{
								smins = '0'+ start.getMinutes();
							}
							else
							{
								smins = start.getMinutes();
							}
							if(end.getMinutes() < 10)
							{
								emins = '0'+ end.getMinutes();
							}
							else
							{
								emins = end.getMinutes();
							}
							startTime = (datesArr[start.getMonth()] + ' ' + start.getDate() + ' ' + start.getHours() + ':' + smins);
							endTime = (datesArr[end.getMonth()] + ' ' + end.getDate() + ' ' + end.getHours() + ':' + emins);
							if(outputFormat ==='artemisjson'){	
								bwt.addRow([
									myObject.result[i].progNowNext.Title,
									myObject.result[i].progNowNext.Description,
									startTime,
									endTime,
									(serviceList[myObject.result[i].progNowNext.ServiceId] + ' ' + myObject.result[i].progNowNext.ServiceId),
									rowButtons( myObject.result[i].progNowNext.ServiceId )
								]);
							}else{
								bwt.addRow([
									myObject.result[i].prognownext.title,
									myObject.result[i].prognownext.description,
									startTime,
									endTime,
									(serviceList[myObject.result[i].prognownext.serviceid] + ' ' + myObject.result[i].prognownext.serviceid),
									rowButtons( myObject.result[i].prognownext.serviceid )
								]);
							}
						}
						break;
					}
					case 'epgJSON':{
						bwt.setHeader(['Program Title', 'Description', 'Start', 'End', 'Service ID','']);
						//len = myObject.result.length;
						for(var i = 0; i < len; i++)
						{
							if(myObject.result[i] != undefined){
								try{
								start = new Date(myObject.result[i].progschedule.start * 1000);
								}catch(e){
									start = new Date(myObject.result[i].progSchedule.Start * 1000);
								}
								try{
								end = new Date((parseInt(myObject.result[i].progSchedule.End) * 1000));
								}catch(e){
									end = new Date((parseInt(myObject.result[i].progschedule.end) * 1000));
								}
								if(start.getMinutes() < 10)
								{
									smins = '0'+ start.getMinutes();
								}
								else
								{
									smins = start.getMinutes();
								}
								if(end.getMinutes() < 10)
								{
									emins = '0'+ end.getMinutes();
								}
								else
								{
									emins = end.getMinutes();
								}
								startTime = (datesArr[start.getMonth()] + ' ' + start.getDate() + ' ' + start.getHours() + ':' + smins);
								endTime = (datesArr[end.getMonth()] + ' ' + end.getDate() + ' ' + end.getHours() + ':' + emins);
								
								//array is held within myObject.result
								bwt.addRow([
									'<a name="'+myObject.result[i].progSchedule.ServiceId+'">' + myObject.result[i].progSchedule.Title + '</a>',
									'<span id="hideMe">'+ myObject.result[i].progSchedule.Description +'</span>',
									startTime,
									endTime,
									(serviceList[myObject.result[i].progSchedule.ServiceId] + ' ' + myObject.result[i].progSchedule.ServiceId),
									rowButtons( myObject.result[i].progSchedule.serviceid )
									]);
							}
						}
						break;
					}
					case 'epgXML':{ // No camelCase with XML
						bwt.setHeader(['Program Title', 'Description', 'Start', 'End', 'Service ID','']);
						//len = myObject.result.length;
						for(var i = 0; i < len; i++)
						{
							var splitArray = myObject.result[i].progschedule.start.split("T", 2);
							splitArray[0] = splitArray[0].split("-", 3);
							splitArray[1] = splitArray[1].split(":", 3);
							start = new Date();
							start.setDate(parseInt(splitArray[0][2]));
							start.setMonth(parseInt(splitArray[0][1]) -1);
							start.setYear(parseInt(splitArray[0][0]));
							start.setHours(parseInt(splitArray[1][0]));
							start.setMinutes(parseInt(splitArray[1][1]));
							start.setSeconds(parseInt(splitArray[1][2]));
							splitArray = myObject.result[i].progschedule.end.split("T", 2);
							splitArray[0] = splitArray[0].split("-", 3);
							splitArray[1] = splitArray[1].split(":", 3);
							end = new Date();
							end.setDate(parseInt(splitArray[0][2]));
							end.setMonth(parseInt(splitArray[0][1]) -1);
							end.setYear(parseInt(splitArray[0][0]));
							end.setHours(parseInt(splitArray[1][0]));
							end.setMinutes(parseInt(splitArray[1][1]));
							end.setSeconds(parseInt(splitArray[1][2]));							
							if(start.getMinutes() < 10)
							{
								smins = '0'+ start.getMinutes();
							}
							else
							{
								smins = start.getMinutes();
							}
							if(end.getMinutes() < 10)
							{
								emins = '0'+ end.getMinutes();
							}
							else
							{
								emins = end.getMinutes();
							}
							startTime = (datesArr[start.getMonth()] + ' ' + start.getDate() + ' ' + start.getHours() + ':' + smins);
							endTime = (datesArr[end.getMonth()] + ' ' + end.getDate() + ' ' + end.getHours() + ':' + emins);
							bwt.addRow([
								myObject.result[i].progschedule.title,
								myObject.result[i].progschedule.description,
								startTime,
								endTime,
								(serviceList[myObject.result[i].progschedule.serviceid] + ' ' + myObject.result[i].progschedule.serviceid),
								rowButtons( myObject.result[i].progschedule.serviceid )
								]);
						}
						break;
					}
					default:
					{
						bwt.setHeader(['Artemis ID', 'Channel Number', 'Channel Name', 'URI', '']);	
						if(!! myObject){					
							//var len = myObject.result.length;
							for( var i = 0; i < len; i++ ) {
								bwt.addRow([
										myObject.result[i].id,
										'<span id="'+ myObject.result[i].id + '">' + myObject.result[i].channelNumber + '</span>',
										'<a name="'+myObject.result[i].id+'">' + myObject.result[i].name + '</a>',
										myObject.result[i].uri,
										rowButtons( myObject.result[i].id )
								]);
								//add to list used to map service name to Artemis ID
								serviceList[myObject.result[i].id] = myObject.result[i].name;
								uriList[myObject.result[i].id] = myObject.result[i].uri;
							}
						}
					}
				}
			}
			element('resultsDiv').innerHTML = bwt.getTableHTML();
			$('a').button().click(function(){
				openMPSocket($(this).attr("name"));
			});			
			//element('epgForm').elements['n10Button'].focus();
        }
		
		function parentElemHeightWidth(elem){
			var parent = {
				height: ($(elem).parent().css("height").split("px", 1)[0]),
				width: ($(elem).parent().css("width").split("px", 1)[0])
			}
			return parent;
		}
		
			//start scan with progress bar
		function scanStart(){
			$("#progressDisplay").append('<div id="searchInfoBox">'+
			'<div id="line1">'+
				'<div id="scanInfoServices"><p>Total Services:&nbsp;<span id="totalServices">0</span>&nbsp;</p></div>'+
				'<div id="scanInfoTransponders"><p>Transponders:&nbsp;<span id="transponders">0</span></p></div><br/>'+
			'</div><br/>'+
			'<div id="line2">'+
				'<div id="scanInfoTVServices"><p>TV Services:&nbsp;<span id="tvServices">0</span>&nbsp;</p></div>'+
			'</div><br/>'+
			'<div id="line3">'+
				'<div id="scanInfoDataServices"><p>Data Services:&nbsp;<span id="dataServices">0</span></p></div>'+
				'<div id="scanInfoRadioServices"><p>Radio Services:&nbsp;<span id="radioServices">0</span></p></div></br>'+
			'</div><br/>'+
			'</div>'+
			'<div id="searchProgress" class="ui-corner-all">'+
			'<div id="progressbarWrap" class="ui-corner-all">'+
			'<div id="progressbar" class="ui-corner-all"></div>'+
			'<div id="progressText">'+
			'<h2><span id="completed">0</span>% Completed</h2>'+
			'</div>'+
			'</div>'+
			'<div id="buttons" align="center">'+
			'<button id="startBtn" onclick="javascript:startIt();">'+
			'Start'+
			'</button>'+
			'</div>'+
			'</div>');
			$("#progressDisplay").wrapInner('<div id="fade" class="background" />');
			$('#exitBtn').button();
			var screenQuarter = (screen.width / 4);
			var aCSS={
				"top": (screen.height / 4),
				"left":(screenQuarter / 2),
				"width": (screenQuarter *3),
				"height": (screen.height / 2),
				"z-index": 1000,
				"background-color": "rgb(0,0,0)",
				"position": "absolute"
			}
			$("#fade").css(aCSS);
			$("#fade").toggleClass("ui-widget-content").draggable();
			var parentCSS = parentElemHeightWidth($("#searchProgress"));
			var width = (parentCSS.width / 2);
			var height = (parentCSS.height / 4);
			var top = (parentCSS.height / 4);
			var left = (parentCSS.width / 4);

			$("div#searchProgress #startBtn").button().focus();
			$("#searchProgress").css("height", height).css("width", width).css("left", left);//.css("top", top);
			$("progressbarWrap").css("top", "200");
			$("#progressbar").progressbar({
				value : startVal
			});
			$("#progressbar").progressbar({
				disabled : true
			});
			$("#progressbar").bind("progressbarchange", function(event, ui) {
				$("div#progressText span#completed").html(myObject.result.progress);
				$("#startBtn").button("destroy");
				$("#searchProgress div#buttons").html('<button id="exitBtn" onclick="javascript:exit();">Cancel</button>');
				$("div#searchProgress div#buttons button#exitBtn").button();
			});
			$("#progressbar").bind("progressbarcomplete", function(event, ui) {
				//clearInterval(goThen);
				$("#progressbar").progressbar({
					disabled : true
				});
				$("#searchProgress").css("box-shadow", "0px 0px 100px 2px #0F0");
				$("#progressText").html("<h2>Done!</h2>").css("color", "red");
				$("#progressbarWrap").animate({
					backgroundColor : "#0F0"
				}, 3000);
			});
		}
		
		function startIt(){
			choice = "deleteSL"
			getEPG();
		}
		
		function exit(){
			continueScan = false;
			$("#progressDisplay").html("");
		}

		
		//get remaining epg for that day
		function fullDayGo(id){
			choice = 'epgJSON';
			getEPG(id);
			dispResults();
		}
		
		//display modal dialog with 2 date fields to allow display of epg within that range
		function setDates(id){
			choice = 'nDays';
			dateID = id;
			$( "#dialog-form" ).dialog( "open" );
		}
		
		
        // get now & next for row id
        function nAndNGo(id) {
			choice = 'nAndN';
			getEPG(id);
			dispResults();
        }

        // delete all the rows in the table
        function clearDB() {
            if(confirm('Clear the entire table?')) {
				$("#resultsDiv").empty();
				myObject = 'undefined';
                dispResults();
            }
        }
        
		//main
        function epgGo(value) {
			choice = value;			
			getEPG();
            dispResults();
		}
		
		function testGo(){
			var q = new Date;
			numCC = 0;
			dur = (parseInt($("#duration").val() * 60 * 1000) + (q.getTime()));
			if($("#rand").is(':checked')){
				timeOut = parseInt(getRandomNumber());
			}else{
				timeOut = (parseInt($("#timeBetween").val()) * 1000);
			}	
			var table = new bwTable();
			table.setHeader(["Channel Name", "Result", " "]);
			ccTest = true;
			for( var id in uriList)
			{
				table.addRow([
					serviceList[id], 
					'<span id="result'+id+'">'+ "Not Started" + '</span>',
					'<span id="loading'+id+'">_</span>'
				]);
			}
			$("#changeChannel-form").html('<div id="uriTable" class="aTable"></div>')
			$("#uriTable").html(table.getTableHTML());
			$("#changeChannel-form").append('<div id="changes"><span id="numChanges">Number of changes: 0</span>'+
			'&nbsp;&nbsp;&nbsp;&nbsp;<span id="toSecs">Timeout = ' +
			(timeOut / 1000)+ ' secs</span></div>');
			$("#changeChannel-form").dialog( "open" );
			
		}
		
		//sleep for delay (in milliseconds)
		function sleep(delay){
			delay += new Date().getTime();
			while((new Date().getTime()) < delay);
		}
		
		function testStart(){	
			var next = false;
			
			if(!!ccTest){
				//set id
				if($("#rand").is(':checked')){
					timeOut = parseInt(getRandomNumber());
					$('#toSecs').html('Timeout = ' + (timeOut / 1000) + ' secs');
				}
				if(ccID != undefined){
					var loadingId = '#loading' + ccID;
					$(loadingId).html('Done').css('color', 'green');
				}
				
				for (ccID in uriList){
					if((ccLastID == undefined) || (ccLastID == uriList[ccID]) || (next == true)){
						if(ccLastID == undefined){
							ccLastID = uriList[ccID];
							firstIndex = uriList[ccID];
							var resultId = '#result' + ccID;
							$(resultId).html("Started").css('color', 'green');
							break;
						}else if((ccLastID == uriList[ccID]) && (next == false)){
							if(uriList[ccID] == uriList[uriList.length -1])
							{
								ccLastID = undefined;
							}else{
								next = true;
							}
						}else if((ccLastID != uriList[ccID]) && (next == true)){
							ccLastID = uriList[ccID];
							next = false;
							break;
						}
					}
				}
				
				ccDuration = $("#duration").val();
				ccTimeout = parseInt($("#timeBetween").val());
				
				if(ccID != undefined){
					var resultId = '#result' + ccID;
					var loadingId = '#loading' + ccID;
					$(resultId).html("Started").css('color', 'green');
					$(loadingId).html('<img src="images/loader.gif" title="running" align="middle" />');
					openMPSocket(ccID);
				}				
			}
		}
		
		function dialogGo(){
			var dates = $("#endD, #startD").datepicker({
				onSelect: function( selectedDate ) {
					var option = this.id == "startD" ? "minDate" : "maxDate",
					instance = $( this ).data( "datepicker" ),
					date = $.datepicker.parseDate(
						instance.settings.dateFormat ||
						$.datepicker._defaults.dateFormat,
						selectedDate, 
						instance.settings 
					);
					dates.not( this ).datepicker( "option", option, date );
					$( "#endD, #startD" ).datepicker( "option", $.datepicker.regional[ "en-GB" ]);
				}	
			});	
			
			$( "#dialog-form" ).dialog({
				autoOpen: false,
				height: 450,
				width: 470,
				modal: true,
				open: function(event, ui) { $(".ui-dialog-titlebar-close").hide(); },
				buttons: {
					"Show Range": function(){
						startDate = new Date($("#startD").datepicker("getDate"));
						endDate = new Date($("#endD").datepicker("getDate"));
						var hrs = parseInt($("#shrs").val());
						var mins = parseInt($("#smins").val());
						startDate.setHours(hrs ? hrs:0);
						startDate.setMinutes(mins ? hrs:0);
						hrs =  parseInt($("#ehrs").val());
						mins =  parseInt($("#emins").val());
						endDate.setHours(hrs ? hrs:0);
						endDate.setMinutes(mins ? mins:0);
						$( this ).dialog( "close" );
					},
					Cancel: function() {
						$( this ).dialog( "close" );
					}
				},
				close: function() {
					getEPG(dateID);
				}
			});
		}
		
		function dialogGet(){
			$("#changeChannel-form").dialog({
				autoOpen: false,
				height: 695,
				width: 400,
				modal: true,
				open: function(event, ui) { $(".ui-dialog-titlebar-close").hide(); },
				buttons: {
					"Go!": function(){
						testStart();					
					},
					Cancel: function(){
						ccTest = false;
						$(this).dialog("close");
						$("#resultsDiv").html("<h1>Number of Successful Channel Changes: "+ numCC + "</h1>");
					}
				},
				close: function(){
				}				
			});
		}
        
        function initDisp() {
            //dispResults();
			dialogGo();
			dialogGet();
			//$("#testBtn").button();	
         }
		
		function epgClear()
		{
			if(confirm('Clear message history?')){
				$('#epgRqResult').empty();
				myObject.length = 0;
			}
		}
		
		function seekGo() {
			choice = 'seek';			
			getEPG();
		}
		
		function getRandomNumber(){
			var min = 5;
			var max = 60;
    		return (Math.floor(Math.random() * (max - min + 1)) + min) * 1000;
		}
		
		function randChecked(){
			if($('#rand').is(':checked')){
				$("#timeBetween").attr('disabled', 'true');
			}else{
				$("#timeBetween").removeAttr('disabled');
			}
		}
		
		function getUrl(){
			return ccURL;
		}
		
		function getTimeout(){
			return ccTimeout;
		}
		
		$(document).ready(function(e) {
			$("#outputSelector").buttonset({ label: "Output Selector" });
			$("div#outputSelector input#xmlRadio").change(function(){
				outputChange('xml');
			});
			$("div#outputSelector input#jsonRadio").change(function(){
				outputChange('json');
			});
			
        	initDisp();
        });