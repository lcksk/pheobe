var timer;
mediaState = 0;

function BDlna_MediaStatus() {
	this.numStreams = null;
	this.streamData = null;
}

function streamData() {
	this.videoCodec = null;
	this.extraVideoCodec = null;
	this.transportUri = null;
}

function updateEvent() {
	if (timer != 0)
		clearInterval(timer);
	timer = setInterval(function () { mediaPlayer.updateProgress(); }, 500);
	mediaPlayer._player.removeEventListener('playing', updateEvent, false);
}

var MediaPlayer = Class.extend({
    _source: null,
    _player: null,
    _controls: null,
    _recordSession: null,
    _maxRate: 99,
    _backgroundImage: null,
    _visibleDuration: 5000,
	title: null,
    timerId: 0,
    init: function (id, title) {
        this._player = document.getElementById(id);
        this._controls = document.getElementById('controls');
        this._controls = document.getElementById('timebar');
		this.playButton_ = document.getElementById('playButton');
        this.speedIndicator_ = document.getElementById('speedIndicator');
        this.playProgress_ = document.getElementById('playProgress');
        this.scrubberButton_ = document.getElementById('scrubberButton');
        this.loadProgress_ = document.getElementById('loadProgress');
        this.currentTime_ = document.getElementById('currentTime');
        this.duration_ = document.getElementById('duration');
        this.speedIndicator_.style.visibility = 'hidden';
		this.title = title;
    },
    showControls: function (timeout) {
		var self = this;
		this._controls.className = "";
		if (typeof this.timerId == "number")
            clearTimeout(this.timerId); 		
		this.timerId = setTimeout(function () {
            self.hideControls();
        }, timeout || this._visibleDuration);
		document.getElementById("title").innerHTML = this.title;
		document.getElementById("info").className = "active";
    },
    hideControls: function () {
		this._controls.className = "hidden";
		document.getElementById("info").className = "hidden";
    },
    load: function (src) {
        this._player.setAttribute('src', src);
        this._player.load();
    },
    play: function () {
        this.playButton_.className = 'play-button';
        this.speedIndicator_.style.visibility = 'hidden';
        this.showControls(this._visibleDuration + 5000);
        var playValue = this._player.currentTime / this._player.duration;
		if (playValue < 1) {     
			this._player.play();
        } 
    },
    pause: function () {
        this.playButton_.className = 'pause-button';
        this._player.pause();
    },
    stop: function () {
		mediaState = BDlna_MediaState_eStopped;
        this.playButton_.className = 'play-button';
        this._player.pause();
        this._player.load();
        this.onEndPlayback();
        this.hideControls();
        document.getElementById("imageViewer").className = "hidden";
		mediaState = BDlna_MediaState_eStopped;
    },
  /*  startRecord: function () {
        if (this._source == null)
            return;
        this._recordSession = this._source.startRecord(this._player.src);
        if (this._recordSession != null)
            this.playButton_.className = 'record-button';
    },
    stopRecord: function () {
        if (this._source == null || this._recordSession == null)
            return;
        this.playButton_.className = 'play-button';
        this._source.stopRecord(this._recordSession);
        this._recordSession = null;
    },*/
    seek: function (time) {
		var newtime = this._player.currentTime + time;
        if (newtime > this._player.duration)
            newtime = this._player.duration;
        else if (newtime < 0)
            newtime = 0;

        this._player.currentTime = newtime;
        this.updateProgress();
    },
    fastForward: function () {
        var currentRate = Math.abs(this._player.playbackRate);
        if (currentRate >= this._maxRate) {
            currentRate = 1.0;
			this._player.playbackRate = currentRate;
            this.speedIndicator_.style.visibility = 'hidden';
        }
        else {
            while (currentRate < this._maxRate) {
                currentRate += 1;
                this._player.playbackRate = currentRate;
                if (this._player.playbackRate == currentRate)
                    break;
            }
            this.speedIndicator_.innerHTML = this._player.playbackRate + "x";
            this.speedIndicator_.style.visibility = 'visible';
        }
    },
    rewind: function () {
        var currentRate = Math.abs(this._player.playbackRate);

        if (currentRate >= this._maxRate) {
            currentRate = 1.0;
            this._player.playbackRate = currentRate;
            this.speedIndicator_.style.visibility = 'hidden';
        }
        else {
            while (currentRate < this._maxRate) {
                currentRate += 1;
                this._player.playbackRate = (currentRate * -1.0);
                if (this._player.playbackRate == (currentRate * -1.0))
                    break;
            }
            this.speedIndicator_.innerHTML = this._player.playbackRate + "x";
            this.speedIndicator_.style.visibility = 'visible';
        }
    },
    onBeginPlayback: function () {
        var self = this;
		if (timer != 0)
			clearInterval(timer);
		this._player.addEventListener('playing', updateEvent, false);
        hideAll();
    },
    onEndPlayback: function () {
        var self = this;
        clearInterval(timer);
        timer = 0;
        showAll();
    },
    formatTime: function (ms) {
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
    },
    updateProgress: function () {
        var playValue = this._player.currentTime / this._player.duration;
		this.playProgress_.setAttribute("value", playValue);
        this.playProgress_.style.width = (playValue * 100) + "%";
		this.scrubberButton_.style.left = (playValue * 100) + "%";
        this.currentTime_.innerHTML = this.formatTime(this._player.currentTime);
		this.duration_.innerHTML = this.formatTime(this._player.duration);
		
		document.getElementById("time").innerHTML = this.formatTime(this._player.currentTime) + "/" + this.formatTime(this._player.duration);
		document.getElementById("rDuration").style.width = (playValue * 100) + "%"; 
        
		if (this._player.buffered.length > 0) {
            var loadValue = this._player.buffered.end(0) / this._player.duration;
            if (loadValue > 1) loadValue = 1;
            this.loadProgress_.setAttribute("value", loadValue);
            this.loadProgress_.style.width = (loadValue * 100) + "%";
        }
        if (playValue >= 1) {
            this._player.load();
            this.onEndPlayback();
        }
    }
});
