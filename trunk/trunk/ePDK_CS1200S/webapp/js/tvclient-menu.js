var topMenu = new Array();
var funcNum;
var _guideTimeoutCount = 0;
var _guideTimeoutinterval = null;
var _guideLoading = false;
var _channelBoxActive=false;
var _title = "";
var _queryTime = null;
var _secsStart = 0;
var _secsEnd = 0;

var MenuItemS = MenuItem.extend({
    onItemSelected: function () {
    }
});

var menuItem0 = new MenuItemS();
var menuItem1 = new MenuItemS();

menuItem0.title = "Channel List";
menuItem1.title = "Channel Guide";

topMenu[0] = menuItem0;
topMenu[1] = menuItem1;

menuItem0.onItemSelected = function () {
	if (_channelGuideActive)
		return;
	hideGuide();
	tvRpc();
};

menuItem1.onItemSelected = function () {
	_guideLoading = true;
	Debug("");
	Debug2("");
	channelBoxShow("&nbsp;Loading guide, please wait...");
	setTimeout(guideTimeout, 10000);

	if (_debug)
	{
		if (_guideTimeoutinterval == null)
		{
			_guideTimeoutCount = 0;
			_guideTimeoutinterval = setInterval("monitorGuideLoad()", 1000);
		}
	}
	
	if (_wsAvail)
	{
		hideMenu();
		hideNowNext();
		tvGuideRpc();
	}
	else
	{
		hideMenu();
		hideNowNext();
		showGuide();
	}
};

function monitorGuideLoad()
{
	_guideTimeoutCount++;
	Debug(_guideTimeoutCount + " secs");
}

function stopMonitorGuideLoad()
{
	if (_guideTimeoutinterval)
	{
		clearInterval(_guideTimeoutinterval);
		_guideTimeoutinterval = null;
	}
	_guideLoading = false;
}

function guideTimeout() {
	if (_guideLoading)
	{
		Debug('Failed to load guide');
		channelBoxHide();
	}
	stopMonitorGuideLoad();
}

function connectTimeout() {
	if (!_initComplete) {
		channelBoxShow("No Channels Found...");
	}
	else {
		if ((!_subtitleActive) && (!_audioActive))	// subtitle sharing channelbox
			channelBoxHide();
	}
	_channelBoxActive = false;
}

function connectLostTimeout() {
	channelBoxHide();
}

function tvRpcInit() {
	var jsonString = JSON.stringify({ "jsonrpc": "2.0", "method": "ITVService::createContext", "id": "1"});
	socket.send(jsonString);
}

function tvRpcInit2() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListSet", 
		"params": { "context" : _context, 
					"payload" : { "base" : "all" } 
				  },
		"id": "4"}));
}

function tvRpcInit3() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListSeek", 
		"params": { "context" : _context, 
					"payload" : {"offset":0, "whence":0} 
				  },
		"id": "5"}));
	
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListGet", 
		"params": { "context" : _context, 
					"payload" : {"limit":500} 
				  },
		"id": "6"}));
}

function tvNowNextRpc(id) {
	// date -s "2011-10-12 13:30"
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method":"ITVService::queryNowNextEventInfo", 
		"params": { "context" : _context, 
					 "payload" : { "id" : id } 
				  },
		"id": "7"}));
}

function tvGetNowNextRpc(id) {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVGuide::queryNowNextEventInfo",        
		"params": { "context" : _context, 
			      "payload" : { "id" : id } 
				  },
		"id": "15"}));
}

function tvUTCTimeGet() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::utcTimeGet", 
		"params": { "context" : _context },
		"id": "12"}));
}

function tvUTCTimeGetMonitor() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::utcTimeGet", 
		"params": { "context" : _context },
		"id": "14"}));
}

function tvGuideRpc() {
	if (!_initComplete) {
		tvRpcInit();
	}
	
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListSeek", 
		"params": { "context" : _context, 
					"payload" : { "offset" : 0, "whence" : 0 } 
				  },
		"id": "5"}));

	tvUTCTimeGet();	// start off stage1
}

function tvGuideRpcStage2()
{
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListGet", 
		"params": { "context" : _context, 
					"payload" : {"limit":_maxChannels} 
				  },
		"id": "13"}));
}

function tvGuideQueryRpc(currentTime)
{
	_queryTime = null;

	if (currentTime != null)
	{
		var start = new Date(currentTime * 1000);
		start.setUTCMinutes(0);
		start.setUTCSeconds(0);
		var end = new Date(start); 
		end.setHours(end.getHours() + (_maxEPGDay * 24));
		if (start.getHours() > 4)
			start.setHours(start.getHours() - 4);	// 4 hrs before
		//if (_debug)
		//	Debug2('query on start=' + start.toUTCString() + ', end=' + end.toUTCString());
		
		_secsEnd = end.getTime() / 1000;
		_secsStart = start.getTime() / 1000;
		_queryTime = " && Start >= " + _secsStart + " && End <= " + _secsEnd;
	}

	if (_useMongoose)
	{
		//setTimeout(function(){tvGuideQueryRpcStage(queryTime);}, 100);
		tvGuideQueryRpcStages(0);
		return;
	}

	for (var i = 0; i < _channels.length; i++) {
		var id = _channels[i]['channel'].id;
		var query = "ServiceId == " + String(id);
		//alert('i=' + i + ', query=' + query + ', len=' + _channels.length);		
		if (_queryTime != null)
			query = query + _queryTime;

		var valid = false;
		var base = 0;
		var limit = 0;
		if ( _secsStart!=0 && _secsEnd!=0) {
			base = _secsStart;
			limit = _secsEnd;
			valid = true;
		}

		socket.send(JSON.stringify({"jsonrpc": "2.0", 
			"method": "ITVGuide::queryEventInfo", 
			"params": { "context" : _context, 
						"payload" : { "query" : {					
											  "sliceRange" : { "valid":false, "base":0, "limit":0 }, 
											  "timeRange" : { "valid":valid, "strict":false, "base":base, "limit":limit },  
											  "serviceIdList": [id],
											  "stringList": [],
											  "stringAnd": false,
											  "stringMatchDescription": false,
											  "genreList": [],
											  "orderByServiceIdPriority": -1,
											  "orderByStartTimePriority": 0
					                            }
					                }
					  },
			"id": String(100+i)}));
	}
}

function tvGuideQueryRpcStages(i)
{
	if (!(i < _channels.length))
		return;	// invalid index
		
	var id = _channels[i]['channel'].id;
	var query = "ServiceId == " + String(id);
	if (_queryTime != null)
		query = query + _queryTime;

	var valid = false;
	var base = 0;
	var limit = 0;
	if ( _secsStart!=0 && _secsEnd!=0) {
		base = _secsStart;
		limit = _secsEnd;
		valid = true;
	}

	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVGuide::queryEventInfo", 
		"params": { "context" : _context, 
			  	    "payload" : { "query" : {					
											  "sliceRange" : { "valid":false, "base":0, "limit":0 }, 
											  "timeRange" : { "valid":valid, "strict":false, "base":base, "limit":limit },  
											  "serviceIdList": [id],
											  "stringList": [],
											  "stringAnd": false,
											  "stringMatchDescription": false,
											  "genreList": [],
											  "orderByServiceIdPriority": -1,
											  "orderByStartTimePriority": 0
					                        }
					            }
				  },
		"id": String(100+i)}));
}

function tvMediaPlayerInit()
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::createInstance", 
		"id": "15"}));
}

function tvStartRpc(uri)
{
	if (_useVideoTag)
	{
		var mediaPlayer = document.getElementById("videoPlayer");
		mediaPlayer.setAttribute('src', uri);
		mediaPlayer.load();
		mediaPlayer.play();
	}
	else
	{
		if (!_proxy)
			return;
			
		MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
			"method": "IMediaPlayer::setDataSource", 
			"params" : {"proxy": _proxy, "dataSource":uri
						},					  
			"id": "11"}));
	}
}

function tvWindowPosRpc(x, y, width, height)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::setVideoWindowPosition", 
		"params" : {"proxy": _proxy, "x":x, "y":y, "width": width, "height":height},					  
		"id": "14"}));
}

function tvScanRpc() {
	if (!_initComplete) {
		tvRpcInit();
	}
	
	/*socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListSet", 
		"params": { "context" : _context, 
					"payload" : { "offset" : 0, "whence" : 0 } 
				  },
		"id": "21"}));*/
}

function tvScanRpcStage2() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVService::serviceListDeleteAll", 
		"params": { "context" : _context, 
					"payload" : { } 
				  },
		"id": "22"}));
}

function tvScanRpcStage3() {
	socket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "ITVConfig::scanBlindStart", 
		"params": { "context" : _context, 
					"payload" : { "sourceId" : 0,
								   "useNIT": -1
					} 
				  },
		"id": "23"}));
}

function tvScanGetInfoRpc(delay)
{
	setTimeout(function(){
		socket.send(JSON.stringify({"jsonrpc": "2.0", 
			"method": "ITVConfig::scanInfoGet", 
			"params": { "context" : _context, 
						"payload" : { "sourceId" : 0 } 
					  },
			"id": "24"}));
	}, delay);
}

function tvSubTitleRpc() {
	if (!_initComplete) {
		tvRpcInit();
	}

	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::getSubtitleAttributes", 
		"params": { "proxy": _proxy },
		"id": "30"}));
}

function tvSubTitleGetStreamRpc() {
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::getSubtitleStream", 
		"params": { "proxy": _proxy, "secondary" : false }, 
		"id": "31"}));
}

function tvSubTitleSetStreamRpc(index, secondary)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::setSubtitleStream", 
		"params": { "proxy": _proxy, 
					"subtitleStreamIndex" : index,
					"secondary" : secondary },
		"id": "32"}));
}

function tvSubTitleSetVisiblityRpc(visible, secondary)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::setSubtitleVisibility", 
		"params": { "proxy": _proxy, 
					"visible" : visible,
					"secondary" : secondary },
		"id": "33"}));
}

function tvGetAudioStreamRpc(secondary)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::getAudioStream", 
		"params": { "proxy": _proxy, 
					"secondary" : secondary },
		"id": "35"}));
}

function tvGetAudioAttributesRpc()
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::getAudioAttributes", 
		"params": { "proxy": _proxy }, 
		"id": "36"}));
}

function tvAudioSetStreamRpc(index, secondary)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::setAudioStream", 
		"params": { "proxy": _proxy, 
					"audioStreamIndex" : index,
					"secondary" : secondary },
		"id": "37"}));
}

function tvAudioVolumeRpc(secondary)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::getAudioVolume", 
		"params": { "proxy": _proxy, 
					"secondary" : secondary },
		"id": "38"}));
}

function tvAudioSetVolumeRpc(volume, secondary)
{
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", 
		"method": "IMediaPlayer::setAudioVolume", 
		"params": { "proxy": _proxy, 
					"volume" : volume,
					"secondary" : secondary },
		"id": "39"}));
}

var tvclientMenu = Menu.extend({
    init: function () {
        this._super(12); // pageSize is 12
        this.refresh();
        this.setItemFocus(0);
        this.selectedIndex = 0;
    },
    goBack: function () {
		var self = this;
		this.hide(function () { self.goHome(); self.show(); });
    },

    getItemCount: function () {
        return topMenu.length;
    },
    goHome: function () {
        this.startIndex = 0;
        this.selectedIndex = -1;
        this.refresh();
        this.setItemFocus(0);
    },
    getItem: function (index) {
		return topMenu[index];
    },

    onItemFocusChanging: function (index) {

    },
    onItemFocused: function (index) {

    },
    onItemSelected: function (index) {
		topMenu[index].onItemSelected();
    }
});


