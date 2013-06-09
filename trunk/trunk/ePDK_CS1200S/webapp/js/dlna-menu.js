var _defaultImages = ["images/dlna256.png", "images/media_folder160x240.png", "images/videos256.png", "images/music256.png", "images/photos256.png"];
var previewActive = false;
var previewAvailable = false;
var selectedIndex = 0;
var previewTimeout = 0;
var pressed = false;
var isInit = true;
var callID = 0;
var mosaic = false;
var pip = false; 
var rootTimerObject;
var recieved = false;

function loadedTotal() {
	this.loadedCount = null;
	this.totalCount = null;
}
var MediaMenu = Menu.extend({
    coverflow: null,
    rootContainer: null,
    container: null,
    sourceTimer: null,
	loadedCount: null,
	sequencer: null,
    init: function (sources) {
        this._super(5); // pageSize is 5
        this.rootContainer = new SourceContainer(sources);
        this.rootContainer.title = "Media Player";
        this.setHeaderText(this.rootContainer.title);
        this.container = this.rootContainer;
        this.coverflow = createCoverflowControl();  
		this.sequencer = new callSequencer();
	    this.loadItems(0);
    },
	reinit: function(sources) {
        this.rootContainer = new SourceContainer(sources);
        this.rootContainer.title = "Media Player";
        this.setHeaderText(this.rootContainer.title);
        this.container = this.rootContainer;
	    this.loadItems(0);
	},
    show: function () {
        this._super();
    },
    hide: function (hideCompleteCallback) {
        this._super(hideCompleteCallback);
    },
    goBack: function () {
        if (this.container.parent != null) {
            var self = this;
            this.hide(function () { self.selectContainerItem(self.container.parent); });
        }
    },
    loadItems: function (startIndex) {
        var chunkSize = 5;
		if (startIndex == 0) {
			chunkSize = 20;
			this.loadedCount = 0;
		}
		menu = this;
		this.container.loadChildren(startIndex, chunkSize, menu);
    },
	populateItems: function(count) {
		this.loadedCount = this.loadedCount + count.loadedCount;
	},
    getItemCount: function () {
        return this.container.children.length;
    },
    getItem: function (index) {
        if (index >= this.getItemCount())
            return null;
        var item = this.container.children[index];
        if (item == null) {
            this.loadItems(index);
		}
		var prefetch = index + 1;
		if ( (prefetch == this.loadedCount) && (prefetch <  this.getItemCount()) ) {
			this.loadItems(prefetch);
		}
        return this.container.children[index];
    },
    createRootTimer: function () {
        var self = this;
		rootTimerObject = this;
    },
    selectContainerItem: function (container) {
        this.setHeaderText(container.title);
        this.coverflow.clearImages();
        this.container = container;
        this.loadItems(0);
        this.goHome();
        this.show();
    },
    onItemFocusChanging: function (index) {
		this.coverflow.moveTo(index);
    },
    onItemFocused: function (index) {
		onMoveCheck(index);
    },
    onItemSelected: function (index) {
        var item = this.container.children[index];
        selectedIndex = index;
        if (item.type == 0) {
            var self = this;
            this.hide(function () { self.selectContainerItem(item); });
        }
        else {
            item.select(this);
        }
    }
});
var MediaObject = MenuItem.extend({
    source: null,
    parent: null,
    info: null,
    poster: null,
    type: -1,
    init: function() {
    },
    select: function() {
    }
});
var MediaContainer = MediaObject.extend({
    children: [],
	menu: null,
    init: function () {
        this.children = [];
        this.type = 0;
        this.poster = _defaultImages[1];
    },
    loadChildren: function (startIndex, count, menu) {
        var item, child;
		this.menu = menu;
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.loadChildrenCallback, [menu, startIndex]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getChildren", "params" : {"source":this.source.udn, "parentId":this.info.id, "startingIndex":startIndex, "count":count}, "id": idString}));
		callID++;
	},
	loadChildrenCallback: function(self, params, myObject, data) {
		var myJSONText = JSON.stringify(data);
		var startIndex = params[1]; 
		var childObjects = myObject.result.list;
		
        var item;
		if( childObjects.length == 0) {
			document.getElementById("mediaMessage").className = "";
			//return since list is empty; Do not process.
			return;
		} else {
			document.getElementById("mediaMessage").className = "hidden";
		}
		for (var i = 0; i < childObjects.length; i++) {
            child = childObjects[i];
			if (child.type == 0)
                item = new MediaContainer();
            else {
                item = new MediaItem();
                item.poster = child.poster;
            }
            item.source = self.source;
            item.parent = self;
            item.title =  child.title;
            item.info = child;
            item.configure(self.menu);
            self.children[startIndex + i] = item;
        }
        self.children.length = myObject.result.totalCount;
		var info = new loadedTotal();
		info.loadedCount = childObjects.length;
		info.totalCount = self.children.length;
		self.menu.populateItems(info);
		self.menu.refresh();
		if(params[1] == 0) {
			self.menu.setItemFocus(0);
			}
		self.menu.coverflow.loadImages(self, 0, childObjects.length, self.children.length, 0);
		if(params[1] == 0)
			self.menu.coverflow.moveTo(0);
		if(params[1] == 0)
			self.menu.goHome();
	},
	itemGetIcon: function(item, sourceObj) { },
    configure: function () { }
});

var SourceContainer = MediaContainer.extend({
    _dlnaSources: null,
    _extSources: null,
    init: function(extSources) {
        this._super();
        this._dlnaSources = [];
        this._extSources = extSources;
    },
    loadChildren: function(startIndex, count, menu ) {
        var source, item;
        this.children = [];
        this._dlnaSources = [];
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.loadChildrenCallback, [menu, startIndex]);
		setTimeout(function() {  socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getSourceList", "params":{"source":{"friendlyName":"Serviio (lamj-PC)","udn":"de561ee5-3b4f-3e69-b2d3-5af1ee699c2b"},"parentId":"3","startingIndex":"0","count":"0"}, "id":idString})); }, 1000);
		callID++;
	},
	loadChildrenCallback: function(self, params, myObject) {
		var menu = 	params[0];
		self._dlnaSources = myObject.result;
		if( self._dlnaSources.length == 0) {
			document.getElementById("serverMessage").className = "";
		} else {
			document.getElementById("serverMessage").className = "hidden";
		}
		for (var i = 0; i < self._dlnaSources.length; i++) {
				source = self._dlnaSources[i];
				item = new MediaContainer();
				item.parent = self;
				item.source = source;
				var title = source.friendlyName + " (" + source.deviceAddress
				item.title = source.friendlyName  + " (" + source.deviceAddress + ")"
				item.info = new Object();
				item.info.id = "0";
				item.children[0] = self;
				self.itemGetIcon(item, source, menu, i);
				if (item.poster == null)
					item.poster = _defaultImages[0];
				self.children.push(item);
			}
			if (self._extSources != null) {
				for (var i = 0; i < self._extSources.length; i++) {
					self.children.push(self._extSources[i]);
				}
			}
			info = new loadedTotal();
			info.loadedCount = self.children.length;
			info.totalCount = self.children.length;
			menu.populateItems(info);
			menu.refresh();
			menu.setItemFocus(0);
			menu.loadedCount = 0;
			isInit = false;
	},
	
	itemGetIcon: function(item, source, menu, i) {
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.itemGetIconCallback, [item, menu, i]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getIconUrl", "params" : {"source":source.udn, "IconType":1, "IconSize":1}, "id": idString}));
		callID++;
	},
	itemGetIconCallback: function(self, params, myObject) {
		var menu = params[1];
		params[0].poster = myObject.result.value;
		menu.coverflow.loadImages(menu.container, 0, menu.container.children.length, menu.container.children.length, 1);
		menu.refresh();
		if(params[2] == 0)
			menu.coverflow.moveTo(0);
	}
});

var MediaItem = MediaObject.extend({
    init: function () {
        this.type = 1;
    },
    select: function (menu) {
		if(pip || mosaic) {
			pip = false;
			mosaic = false;
			stopAllPreviews();
		}
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.selectCallback, [menu]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getResources", "params" : {"source":this.source.udn, "objectId":this.info.id}, "id": idString}));
		callID++;
	},
	selectCallback: function (self, params, myObject) {
        var resources = myObject;
		var playerName = self.playerName;
        var url = resources.result[0].url;
		var protocolInfo = resources.result[0].protocolInfo;
		if(protocolInfo.slice(0,16) == "http-get:*:image") {
			launchMedia(self.source.udn, url, "imageViewer", self.title);
		} else {
			launchMedia(self.source.udn, url, playerName, self.title);
		}
	},
    configure: function (menu) {
		this.playerName = "videoPlayer";
		switch (this.info.upnpClass) {
			case "object.item.videoItem":
				if (this.poster == "") {
					this.poster = _defaultImages[2];
				} 
				this.playerName = "videoPlayer";
				break;
			case "object.item.audioItem.musicTrack":
				if (this.poster == "") {
					this.poster = _defaultImages[3];
				}
				this.playerName = "audioPlayer";
				break;
			case "object.item.imageItem.photo":
			   this.setImagePoster(menu);
				this.playerName = "imageViewer";
				break;
		}
    },
	setImagePoster: function (menu) {
        var protocolInfo;
        var lrgIndex = medIndex = smIndex = tnIndex = -1;
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.setImagePosterCallback, [menu]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getResources", "params" : {"source":this.source.udn, "objectId":this.info.id}, "id": idString}));
		callID++;
	},
	setImagePosterCallback: function(self, params, myObject) {
		var resources = myObject;
	
		  for (var i = 0; i < resources.length; i++) {
            protocolInfo = resources[i].protocolInfo;
            if (protocolInfo.match("JPEG_TN|PNG_TN")) {
                tnIndex = i;
                break; // Prefer thumbnail
            }
            else if (protocolInfo.match("JPEG_SM|PNG_SM"))
                smIndex = i;
            else if (protocolInfo.match("JPEG_MED|PNG_MED"))
                medIndex = i;
            else if (protocolInfo.match("JPEG_LRG|PNG_LRG"))
                lrgIndex = i;
        }
        if (tnIndex >= 0)
            this.poster = resources[tnIndex].url;
        else if (smIndex >= 0)
            this.poster = resources[smIndex].url;
        else if (medIndex >= 0)
            this.poster = resources[medIndex].url;
        else if (lrgIndex >= 0)
            this.poster = resources[lrgIndex].url;
        else {
            this.poster = _defaultImages[4];
        }
	}, 
	preview: function(index) {
		if(pip || mosaic) {
			pip = false;
			mosaic = false;
			stopAllPreviews();
		}
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.previewCallback, [index]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getResources", "params" : {"source":this.source.udn, "objectId":this.info.id}, "id": idString}));
		callID++;
	},
	previewCallback: function(self, params, myObject) {
		setPipHighlight();
		var resources = myObject;
		var protocolInfo = resources.result[0].protocolInfo;
		if(protocolInfo.slice(0,16) == "http-get:*:image") {
			document.getElementById("message").className = '';
			setTimeout(function() {clearPreviewMessage(); }, 1500);
			return;
		}
        var url = resources.result[0].url + "?decoderType=VIDEO_DECODER_PIP";
		var div;
		if(resources.result[7]) {
			var urlSmall = resources.result[7].url;
		}
		if(_videoBrowser.menu.getItemCount() < 6)
			div = _videoBrowser.menu._menu.children[params[0]];
		else if(params[0] < 4)
			div = _videoBrowser.menu._menu.children[params[0]];
		else if(params[0] + 1 > _videoBrowser.menu.getItemCount())
			div = _videoBrowser.menu._menu.children[5];
		else if(params[0] + 2 > _videoBrowser.menu.getItemCount())
			div = _videoBrowser.menu._menu.children[4];
		else
			div = _videoBrowser.menu._menu.children[3];
		
		var imgItem = div.children[1];
		var videoItem = div.children[3];
		videoItem.removeEventListener('ended', arguments.callee, false);
		videoItem.className = "preview";  
		imgItem.className = 'imgIcon';
		videoItem.setAttribute('src', url);
        videoItem.load();
		videoItem.className = "previewPlay";
		imgItem.className = "imgIcon imgHidden";
		videoItem.addEventListener('webkitTransitionEnd',
            function () {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
				this.play();
				videoItem.addEventListener('ended', function() { this.removeEventListener('ended', arguments.callee, false); videoItem.className = "preview";  imgItem.className = 'imgIcon'; }, false);
            }, false);
	},
	fullPreview: function(index) {
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.fullPreviewCallback, [i + 1]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getResources", "params" : {"source":this.source.udn, "objectId":this.info.id}, "id": idString}));
		callID++;
	},
	fullPreviewCallback: function(self, params, myObject) {
		setPipHighlight();
		var resources = myObject;
		var div = _videoBrowser.menu._menu.children[params[0]];
		var url;
		var useURL = false;
		var protocolInfo = resources.result[0].protocolInfo;
		if(protocolInfo.slice(0,16) != "http-get:*:image") {		
			for(i = 0; i < resources.result.length; i++) {
				useURL = false;
				var tokenResolution = resources.result[i].resolution.split("x");
				var pUrl = resources.result[i].url;
				var x = parseInt(tokenResolution[0]);
				var y = parseInt(tokenResolution[1]);
				if(x < 401) {
					if(y < 301) {
						if(pUrl.slice(0,4) == "http")
							useURL = true;
					}
				}
				if(useURL) {
					url = pUrl + "?mosaic=" + params[0];
					break;
				}
			}
		}
		if(useURL) {
			var imgItem = div.children[1];
			var videoItem = div.children[3];
			videoItem.setAttribute('src', url);
			videoItem.load();
			videoItem.className = "previewPlay";
			imgItem.className = "imgIcon imgHidden";
			videoItem.addEventListener('webkitTransitionEnd',
				function () {
					this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
					this.play();
					videoItem.addEventListener('ended', function() { this.removeEventListener('ended', arguments.callee, false); videoItem.className = "preview";  imgItem.className = 'imgIcon'; }, false);
				}, false);	
		} else {
			var text = div.children[4];
			text.className = 'pipIndicator pipPosition';
			setTimeout(function() {clearPipMessage(); }, 5000);
			return;
		}
	},
	pushMedia: function(index) {
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.pushMediaCallbackMP, [index]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getResources", "params" : {"source":this.source.udn, "objectId":this.info.id}, "id": idString}));
		callID++;
	},
	pushMediaCallback: function(self, params, myObject) {
		var resources = myObject;
        var url = resources.result[0].url;
		var idString = callID + "";
		menu.sequencer.push(idString, 0, pushCallback, [menu]);
		MPsocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::setDataSource", "params" : {"dataSource":url}, "id": idString}));
		callID++;
	},
	pushMediaCallbackMP: function(self, params, myObject) {
		 var resources = myObject;
        var url = resources.result[0].url;
		var currentUrl = document.location.href;
		currentUrl = currentUrl.slice(0, currentUrl.length - 14);
		url = currentUrl + "mediaPlayer.html?" + url;
		bamSocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IWindowManager::launch", "params":{ "serverPID" : "0", "applicationType" :"Webapp", "applicationURL" : url, "applicationParameters" : "", "compositingParameters" : { "dimensions" : { "w" : 1280, "h" : 720 }, "visibility" : true, "opacity" : 255, "zorder" : 2, "location" : { "x" : 0, "y" : 0, "w" : 1280, "h" : 720 }, "tunnelCapable" : false}, "giveFocus" : true, "enableDebug" : true, "isStandalone": false }, "id": "2"}));
	}
});

var ViewItem = MediaObject.extend({
    _viewName: null,
    init: function(viewName, title, poster) {
        this._super();
        this._viewName = viewName;
        this.title = title;
        this.poster = poster;
    },
    select: function() {
        var self = this;
    }
});
function pushFocusedMedia() {
	var menu = _videoBrowser.menu;
	menu.container.children[menu.focusedIndex].pushMedia(menu.focusedIndex + 1);
}

function pushCallback(self, params, myObject) {
	var idString = callID + "";
	params[0].sequencer.push(idString, 0, noCallback, 0);
	isPreparer = true;
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::prepare", "id": idString}));
	callID++;
}

function prepareMedia() {
	var idString = callID + "";
	menu.sequencer.push(idString, 0, noCallback, 0);
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::prepare", "id": idString}));
	callID++;
}

function startMedia() {
	var idString = callID + "";
	menu.sequencer.push(idString, 0, noCallback, 0);
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::start", "id": idString}));
	callID++;
}

function resetMedia() {
	var idString = callID + "";
	menu.sequencer.push(idString, 0, noCallback, 0);
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::reset", "id": idString}));
	callID++;
}

function stopMedia() {
	var idString = callID + "";
	menu.sequencer.push(idString, 0, noCallback, 0);
	MPsocket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaPlayer::reset", "id": idString}));
	callID++;
}

function noCallback(self, params, myObject) { }

function startFullPreview() {
	for(i = 1; i < _videoBrowser.menu._menu.children.length; i++) {
	_videoBrowser.menu._menu.children[i].children[3].className = "previewPlay";
	_videoBrowser.menu._menu.children[i].children[3].addEventListener('webkitTransitionEnd',
            function () {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
				this.play();
            }, false);
	}
}
function startOnePreview() {
	var menu = _videoBrowser.menu;
	if(!menu.container.children[menu.focusedIndex].preview ) {
		document.getElementById("message").className = '';
		setTimeout(function() {clearPreviewMessage(); }, 1500);
	}
	menu.container.children[menu.focusedIndex].preview(menu.focusedIndex + 1);
}

function startFullPreviewReal() {
	var menu = _videoBrowser.menu;
	for(i = 0; i < 5; i++) {
		if(!menu.container.children[menu.startIndex + i].fullPreview ){
			document.getElementById("message").className = '';
			setTimeout(function() {clearPreviewMessage(); }, 1500);
		} else
			menu.container.children[menu.startIndex + i].fullPreview(i + 1);
		}
}

function stopAllPreviews() {
	for(i = 1; i < _videoBrowser.menu._menu.children.length; i++) {
	var videoItem = _videoBrowser.menu._menu.children[i].children[3];
	var imgItem = _videoBrowser.menu._menu.children[i].children[1];
	videoItem.className = "preview";
	imgItem.className = 'imgIcon';
	//videoItem.pause(); Do not pause here, expensive media engine call
	videoItem.src = 'http://127.0.0.1/bogus';
	videoItem.load();
	this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
	this.removeEventListener('ended', arguments.callee, false);
	videoItem.addEventListener('webkitTransitionEnd',
            function () {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);	
            }, false);
	}
}

function stopMiddle() {
	var videoItem = _videoBrowser.menu._menu.children[3].children[3];
	var imgItem = _videoBrowser.menu._menu.children[3].children[1];
	videoItem.className = "preview";
	imgItem.className = 'imgIcon';
	videoItem.load();
	videoItem.removeEventListener('webkitTransitionEnd', arguments.callee, false);
	videoItem.removeEventListener('ended', arguments.callee, false);
}

function onMoveCheck(index) {
	if(_videoBrowser.menu.getItem(index).element.children[3].paused)
		setPosterHighlight();
	else
		setPipHighlight();
}

function setPipHighlight() {
	document.getElementById("highlight").className = "highlight2";
}

function setPosterHighlight() {
	document.getElementById("highlight").className = "highlight";
}

function formatTime(ms) {
        var totalSeconds = ms ;
        var hours = parseInt(totalSeconds / 3600);
        var minutes = parseInt((totalSeconds % 3600) / 60);
        var seconds = parseInt(totalSeconds % 60);
        var result = "";
        if (hours > 0)
            result += hours.toString() + ":";
        if (minutes < 10)
            result += "0";
        result += minutes.toString() + ":";
        if (seconds < 10)
            result += "0";
        result += seconds.toString();
        return result;
}

function clearPipMessage() {
	for(i = 1; i < 6; i++) {
		var div = _videoBrowser.menu._menu.children[i];
		var text = div.children[4];
		text.className = 'hidden pipPosition';
	}
}

function clearPreviewMessage() {
		document.getElementById("message").className = 'hidden';
}
