var mediaPlayer = null;
var menuActive = true;
var infoActive = false;
var plusActive = false;
var decoderActive = false;
var enablePip = true;
var statusActive = false;
var BDlna_MediaState = 1;
var flip = false;
var remoteInterval;
var keysLocked = false;

var BDlna_MediaState_eNoMediaPresent = 0;
var BDlna_MediaState_eStopped = 1;
var BDlna_MediaState_ePlaying = 2;
var BDlna_MediaState_ePausedPlay = 3;
var BDlna_MediaState_eTransitioning = 4;
var BDlna_MediaState_eRecording = 5;
var BDlna_MediaState_ePausedRecording = 6;

function clearRemoteInterval() {
	setTimeout(function () { keysLocked = false; }, 300);
}

var Dlna = Class.extend({
    menu: null,
    init: function (sources) {
        this.attachEvents();
        this.menu = new MediaMenu(sources);
		var self = this;
		self.menu.show();
    },
    attachEvents: function () {		
        var self = this;
        var keyDown = function (evt) {
			if(keysLocked)
				return false;
			keysLocked = true;
			clearRemoteInterval();
            if (!menuActive && !infoActive) {
                if (mediaPlayer)
                    mediaPlayer.showControls(3000);
            }
			var stoppingMosaic = false;
			if(mosaic) {
				stopPreview();
				stoppingMosaic = true;
			}
            switch (evt.keyCode) {
                case VK_OK:
                    if (menuActive)
                        self.menu.select();
                    break;
                case VK_DOWN:
                    if (menuActive)
                        self.menu.select();
                    break;
                case VK_UP:
                case VK_LAST:
					stopPreview();
                    if (menuActive)
                        self.menu.goBack();
                    break;
				case VK_LEFT:
					stopPreview();
                    if (menuActive)
                        self.menu.moveUp();
                    break;
				 case VK_RIGHT:
					stopPreview();
                    if (menuActive)
                        self.menu.moveDown();
                    break;
                case VK_CHANNEL_UP:
					stopPreview();
					pressed = true;
                    if (menuActive && !infoActive)
                        self.menu.pageUp();
                    break;
                case VK_CHANNEL_DOWN:
					stopPreview();
                    if (menuActive && !infoActive)
                        self.menu.pageDown();
                    break;
                case VK_EXIT:
                    break;
                case VK_PLAY:
                    mediaPlayer.showControls(3000);
                    mediaPlayer.play();
                    break;
                case VK_PAUSE:
                    mediaPlayer.showControls(3000);
                    mediaPlayer.pause();
                    break;
                case VK_STOP:
                    if (mediaPlayer) {
                        mediaPlayer.stop();
                    }
                    else {
                        showAll();
                        document.getElementById("imageViewer").className = "hidden";
                    }
                    break;
                case VK_INFO:
					if (menuActive)
                        toggleInfo();
                    break;
				case 68: //d
					DEBUG = !DEBUG;
					break; 
				case 403: //red
					togglePreview();
					break;
				case 404:  //green
					if(stoppingMosaic)
						break;
					greenKey();
					break;
				case 405: //blue
					break;
				case 78: //n
				case 38:
				case 405:
					 pushFocusedMedia();
					break; 
                case VK_FASTFORWARD:
                    mediaPlayer.showControls(3000);
                    mediaPlayer.fastForward();
                    break;
                case VK_REWIND:
                    mediaPlayer.showControls(3000);
                    mediaPlayer.rewind();
                case VK_SEEK_LEFT:
                    mediaPlayer.showControls(3000);
                    mediaPlayer.seek(-10);
                    break;
                case VK_SEEK_RIGHT:
                    mediaPlayer.showControls(3000);
                    mediaPlayer.seek(30);
                    break;
                case VK_GUIDE:
                    if (menuActive) {
                        hideAll();
                    }
                    else {
                        showAll();
                    }
                    break;
                case VK_RECORD:
                    mediaPlayer.showControls(3000);
                    if (mediaPlayer._recordSession == null)
                        mediaPlayer.startRecord();
                    else
                        mediaPlayer.stopRecord();
                    break;
                default:
                    return true;
            }
            if (evt.stopPropagation)
                evt.stopPropagation();
            return false;
        }
        document.onkeydown = function (evt) {
            return keyDown(evt);
        };
    }
});

function getMediaType() {
    var search = document.location.search;
    var i = search.indexOf("?");
    if (i < 0)
        return null;
    return search.substring(i + 1);
};

function launchMedia(udn, url, playerName, title) {
	var img = document.getElementById("imageViewer");
    // always stop audio first
    document.getElementById("audioPlayer").load();
	img.onload = imageOnLoad;
    img.className = "hidden";

    if(playerName !="imageViewer") {
        mediaPlayer = new MediaPlayer("videoPlayer", title);
        mediaPlayer._source = source;
        mediaPlayer.load(url);
        mediaPlayer.play();
        mediaPlayer.onBeginPlayback();
    } else {
        document.getElementById("videoPlayer").load();
        img.src = url;
        hideAll();
    }
}

function startPreview(container, index) {
	if ( enablePip) {
		var url1 = container.children[index].source.getResources(container.children[index].info.id)[0].url;
        url1 = url1 + "?decoderType=VIDEO_DECODER_PIP";
		document.getElementById("preview").setAttribute('src', url1);
		document.getElementById("preview").load();
		document.getElementById("preview").play();
		previewActive = true;
	}
}
function stopPreview() {
	if ( enablePip ) {
		document.getElementById("preview").pause();
		document.getElementById("preview").load();
	}
}
function getPreviewIndex() {
	return _videoBrowser.menu.focusedIndex;
}
function hasInfo() {
	var mediaObject = _videoBrowser.menu.container.children[selectedIndex];
	return (mediaObject.type > 0);
}

function toggleInfo() {
	if(infoActive)
			document.getElementById("legend").className = "hidden";
	else
		document.getElementById("legend").className = "";
	infoActive = !infoActive;
}
function toggleInfoPlus() {
    if (!plusActive) {
		var index = getPreviewIndex();
        document.getElementById("preview").className = "previewPlus";
        document.getElementById("menu").className = "menu hidden";
        document.getElementById("plus").className = "plusActive";
        document.getElementById("plusDescription").innerHTML = _videoBrowser.menu.container.children[index].info.description;
        document.getElementById("plusTitle").innerHTML = _videoBrowser.menu.container.children[index].info.title + " (2010)";

        var actors = "";
        var actorsRaw = _videoBrowser.menu.container.children[index].info.actors;
        var i = 0;
        var commaCount = 0;
        while (i != actorsRaw.length && commaCount != 3) {
            if (actorsRaw.charAt(i) == ',') {
                commaCount++;
                if (commaCount == 3)
                    break;
            }
            actors = actors + actorsRaw.charAt(i);
            i++;
        }
        document.getElementById("plusActors").innerHTML = actors;
        document.getElementById("plusDirector").innerHTML = _videoBrowser.menu.container.children[index].info.directors;
		plusActive = true;
    } 
	else {
		undoPlus();
    }
}
function undoPlus() {
    document.getElementById("plus").className = "plusHidden";
    document.getElementById("preview").className = "previewNormal";
    document.getElementById("menu").className = "menu";
    plusActive = false;
}
function hideAll() {
	document.getElementById('coverflow').style.display = "none";
    document.getElementById("menu").className = "menu hidden";
	document.getElementById("footer").className = "hidden";
	document.getElementById("divider").className = "hidden";
	document.getElementById("helpButton").className = "hidden";
	if(infoActive)
		toggleInfo();
	previewActive = false;
    menuActive = false;
}
function showAll() {
	document.getElementById('coverflow').style.display = "inherit";
	document.getElementById("footer").className = "";
    document.getElementById("menu").className = "menu";
	document.getElementById("helpButton").className = "";
	onMoveCheck(_videoBrowser.menu.focusedIndex);
    menuActive = true;
    if (infoActive)
        toggleInfo();
}

window.onunload = function() {
    if (mediaPlayer != null)
        mediaPlayer.stopRecord();
};

function imageOnLoad() {
    var img = document.getElementById("imageViewer");
    img.style.width = "";
    img.style.height = "";
    img.style.left = "";
    img.style.top = "";
    var y, x, left, top, height, width;
    if (img.height <= 720 && img.width <= 1280) {
        y = (720 - img.height) / 2;
        x = (1280 - img.width) / 2;
        left = x + "px";
        top = y + "px";
        img.className = "active";
        img.style.left = left;
        img.style.top = top;
    } else if ((img.width / img.height) > (16 / 9)) {
        img.style.width = "1280px";
        height = 1280 * (img.height / img.width);
        img.style.height = height + "px";
        y = (720 - height) / 2;
        img.style.top = y + "px";
        img.className = "active";
    } else if ((img.width / img.height) < (16 / 9)) {
        img.style.height = "720px";
        width = 720 * (img.width / img.height);
        img.style.width = width + "px";
        x = (1280 - width) / 2;
        img.className = "active";  
    } else {
        img.className = "active";
        img.style.width = "1280px";
        img.style.height = "720px";
    }
}

function infoKey() {
	if(menuActive) {
		if(pip || mosaic) {
			pip = false;
			mosaic = false;
			stopAllPreviews();	
			setPosterHighlight();
		} else {
			startOnePreview();
			pip = true;
		}
	}
}

function guideKey() {
    if (menuActive) {
        hideAll();
    } else {
        showAll();
    }
}

function greenKey() {
	var vid = document.getElementById('videoPlayer');
	if(mosaic) {
		stopAllPreviews();
		mosaic = false;
		setPosterHighlight();
	} else {	
		if(pip) {
			stopAllPreviews();
			pip = false;
		}
		if(vid.paused || vid.stopped) {
			startFullPreviewReal();
			mosaic = true;
		} else {
			document.getElementById("message").className = '';
			setTimeout(function() {clearPreviewMessage(); }, 1500);
		}
	}
}

function stopPreview() {
	if(pip || mosaic) {
		pip = false;
		mosaic = false;
		stopAllPreviews();	
		setPosterHighlight();
	}
}

function togglePreview() {
	if(menuActive) {
		if(pip || mosaic) {
			stopPreview();
		} else {
			startOnePreview();
			pip = true;
		}
	}
}
