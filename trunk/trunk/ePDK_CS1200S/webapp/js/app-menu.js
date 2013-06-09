//Currently can only load initial list once;  Must refresh after that;

var _defaultImages = ["images/dlna256.png", "images/folder256.png", "images/videos256.png", "images/music256.png", "images/photos256.png"];
var previewActive = false;
var previewAvailable = false;
var selectedIndex = 0;
var previewTimeout = 0;
var pressed = false;
var isInit = true;
var callID = 0;

var rootTimerObject;

function loadedTotal() {
	this.loadedCount = null;
	this.totalCount = null;
}

var ApplicationMenu = Menu.extend({
    rootContainer: null,
    container: null,
    sourceTimer: null,
	loadedCount: null,
	sequencer: null,
    init: function (sources) {
        this._super(12); // pageSize is 12
        this.rootContainer.title = "Applications";
        this.setHeaderText(this.rootContainer.title);
        this.container = this.rootContainer;
        this.sequencer = new callSequencer();
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
        else {
           // $Navigator.navigateTo("menu");
        }
    },
    loadItems: function (startIndex) {
        var chunkSize = 5;
        if (startIndex == 0) {
            chunkSize = 20;
            this.loadedCount = 0;
        }
        menu = this;
        //alert("loading children...");
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
			alert("load item from get Item");
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
        this.container = container;
		//alert("select container item?");
        this.loadItems(0);
        this.goHome();
        this.show();
    },
    onItemFocusChanging: function (index) {
		//alert("moving to:");
		//alert("move to return...");
    },
    onItemFocused: function (index) {

    },
    onItemSelected: function (index) {
		//alert("selected....");
        var item = this.container.children[index];
        selectedIndex = index;
        if (item.type == 0) {
			//alert("item type 0");
            var self = this;
            this.hide(function () { self.selectContainerItem(item); });
        }
        else {
            item.select(this);
        }
    }
});

var ApplicationObject = MenuItem.extend({
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

var ApplicationContainer = ApplicationObject.extend({
    children: [],
	menu: null,
    init: function () {
        this.children = [];
        this.type = 0;
        this.poster = _defaultImages[1];
    },
    loadChildren: function (startIndex, count, menu) {
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
		for (var i = 0; i < childObjects.length; i++) {
			
            child = childObjects[i];
			if (child.type == 0)
                item = new ApplicationContainer();
            else {
                item = new ApplicationItem();
                //item.poster = child.poster;
            }
            item.source = self.source;
            item.parent = self;
            item.title = child.title;
            item.info = child;
            item.configure();
            self.children[startIndex + i] = item;
        }
        self.children.length = myObject.result.totalCount;

		var info = new loadedTotal();
		info.loadedCount = childObjects.length;
		info.totalCount = self.children.length;
		self.menu.populateItems(info);
		self.menu.refresh();
		self.menu.setItemFocus(0);
	},
    configure: function () { }
});

var ApplicationItem = ApplicationObject.extend({
    init: function () {
        this.type = 1;
    },
    select: function (menu) {
		var idString = callID + "";
		menu.sequencer.push(idString, this, this.selectCallback, [menu]);
		socket.send(JSON.stringify({"jsonrpc": "2.0", "method": "IMediaSource::getResources", "params" : {"source":this.source.udn, "objectId":this.info.id}, "id": idString}));
		callID++;
	},
	selectCallback: function (self, params, myObject) {
		//alert("case 2");
		//var myJSONText = JSON.stringify(data);
		//alert(myJSONText);
        var resources = myObject;
		var playerName = self.playerName;
        var url = resources.result[0].url;

        launchApplication(self.source.udn, url, playerName);
	
	},
    configure: function () {
		//TODO: pick media type from DLNA information
		this.playerName = "videoPlayer";
		//alert(this.info.upnpClass);
     //   if (parts.length > 2) {
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
                case "object.item.imageItem":
                   // this.setImagePoster();
                    this.playerName = "imageViewer";
                    break;
            }
        
    }
 /*   setImagePoster: function () {
        var protocolInfo;
        var lrgIndex = medIndex = smIndex = tnIndex = -1;
        var resources = this.source.getResources(this.info.id);
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
    }*/
});

var ViewItem = ApplicationObject.extend({
    _viewName: null,
    init: function(viewName, title, poster) {
        this._super();
        this._viewName = viewName;
        this.title = title;
        this.poster = poster;
    },
    select: function() {
        var self = this;
        //setTimeout(function() { $Navigator.navigateTo(self._viewName); }, 1);
    }
});


