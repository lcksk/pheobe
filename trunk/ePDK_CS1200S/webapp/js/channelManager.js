//array of channels for use in manager
var _channelSelection = new Array();
var _selectedChannelIndex = -1;
var _channelBoxInterval = null;
var _channelBoxIntervalCount = 0;

var channel0 = new ManagerItem();
var channel1 = new ManagerItem();
var channel2 = new ManagerItem();
channel0.title = 'dummy1';
channel1.title = 'dummy2';
channel2.title = 'dummy3';

_channelSelection.onItemSelected = function(index) {
	var uri = _channelSelection[index].uri;
	var title = _channelSelection[index].title;
	var id = _channelSelection[index].id;
	//if (_debug)
	//	Debug('id=' + _channelSelection[index].id + ', uri=' + uri);
	channelBoxShow(title);
	_channelBoxIntervalCount = 0;
	if (_channelBoxInterval == null)
		_channelBoxInterval = setInterval("MonitorChannelBox()", 1000);
	_title = title;
	_channelBoxActive = true;

	tvStartRpc(uri); 		
	tvGetNowNextRpc(id); 	
}

channel0.onItemSelected = function () {
};

//_channelSelection[0] = channel0;
//_channelSelection[1] = channel1;
//_channelSelection[2] = channel2;

var CmMenu = Manager.extend({
    init: function () {
        this._super(12); // pageSize is 12
        this.refresh();
        this.setItemFocus(0);
    },
    goBack: function () {
        var self = this;
        this.hide(function () { self.goHome(); self.show(); });
    },

    getItemCount: function () {
        return _channelSelection.length;
    },
    goHome: function () {
        this.startIndex = 0;
        this.selectedIndex = -1;
        this.refresh();
        this.setItemFocus(0);
    },
	moveRight: function () {
		var index = this.focusedIndex;
		if (index != -1) {
			addNowNext(index);
		}
	},
    getItem: function (index) {
        return _channelSelection[index];
    },

    onItemFocusChanging: function (index) {

    },
    onItemFocused: function (index) {

    },
    onItemSelected: function (index) {
		//if (index == 0)
		//	_channelSelection[index].onItemSelected();
		_channelSelection.onItemSelected(index);	
        _selectedChannelIndex = index;
		
		//var player = document.getElementById("video");
		//var src = "http://10.13.135.137:10243/WMPNSSv4/2983551471/1_ezM2MEZFQzcxLTdFQzktNDAyQS05NDkyLTlBOUNCOTVFNjUwNH0uMC44.mp4";
        //player.setAttribute('src', src);
        //player.load();
		//player.play();
    }
});


//function that launches popup for editing a channel
function addChannelPopupLoad() {
    puActive = true;
    document.getElementById("addChannel").className = 'popupActive';
    selectedForm = document.getElementById('addChannelForm');
    
}





