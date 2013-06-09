var delay = 500;
var interval = new Array();
var intervalStatus = new Array();
var samplesCount = new Array();
var simlulate = false;

// corresponds to bdlna_types.h
var BMedia_FrontendMode_eDvb = 0;
var BMedia_FrontendMode_eDss = 1;
var BMedia_FrontendMode_eDcii = 2;
var BMedia_FrontendMode_eQpskTurbo = 3;
var BMedia_FrontendMode_eTurboQpsk=BMedia_FrontendMode_eQpskTurbo;
var BMedia_FrontendMode_e8pskTurbo = 4;
var BMedia_FrontendMode_eTurbo8psk=BMedia_FrontendMode_e8pskTurbo;
var BMedia_FrontendMode_eTurbo = 5;
var BMedia_FrontendMode_eQpskLdpc = 6;
var BMedia_FrontendMode_e8pskLdpc = 7;
var BMedia_FrontendMode_eLdpc = 8;
var BMedia_FrontendMode_eBlindAcquisition = 9;
var BMedia_FrontendMode_e16 = 10;
var BMedia_FrontendMode_e32 = 11;
var BMedia_FrontendMode_e64 = 12;
var BMedia_FrontendMode_e128 = 13;
var BMedia_FrontendMode_e256 = 14;
var BMedia_FrontendMode_e512 = 15;
var BMedia_FrontendMode_e1024 = 16;
var BMedia_FrontendMode_e2048 = 17;
var BMedia_FrontendMode_e4096 = 18;
var BMedia_FrontendMode_eAuto_64_25 = 19;

var BMedia_FrontendQamAnnex_eA = 0;
var BMedia_FrontendQamAnnex_eB = 1;

for (var i = 0; i < 8; i++) {
    interval[i] = false;
    intervalStatus[i] = false;
    samplesCount[i] = 0;
}

function getNewValue(channel) {
    var numSamples = 32;
    var softDecision;
    var softDecisionObjects
    var samples = new Array();

    if (simlulate) {
        return getNewValueSimulate(channel);
    }

    softDecisionObjects = $MediaPlayer.getFrontendSoftDecision(numSamples, channel);

    for (var i = 0; i < softDecisionObjects.length; i++) 
    {
        softDecision = softDecisionObjects[i];
        if ((softDecision.i < 0) || (softDecision.i >= 256 - 1))
            softDecision.i = 1;
        if ((softDecision.q < 1) || (softDecision.q >= 256-1))
            softDecision.q = 1;
        //alert('i=' + softDecision.i + ', q=' + softDecision.q);
        var xy = new Array(softDecision.i + 1, softDecision.q + 1);
        samples[i] = xy;
        samplesCount[channel]++;
    }
    return samples;
}

function BDlna_MediaFrontendStatus() 
{
    this.numDS = null;
    this.streamData = null;
}

function streamData() 
{
    this.mode = null;
    this.symbolRate = null;
    this.snrEstimate = null;
    this.reacquireCount = null;
    this.frequency = null;
    this.timeElapsed = null;

	// cable specifics 
	this.receiverLock = null;
	this.fecLock = null;
	this.dsChannelPower = null;
	this.annex = null;
}

function getNewValueSimulate(channel) {
    var samples = new Array();

    for (var i = 0; i < 16; i++) {
        var x = Math.round(Math.random() * 256);
        var y = Math.round(Math.random() * 256);
        var xy = new Array(x, y);
        samples[i] = xy;
        samplesCount[channel]++;
    }

    return samples;
}

function initialDraw(channel, name) {
    var myCanvas = document.getElementById(name); 
    var ctx = myCanvas.getContext('2d');
    ctx.clearRect(0, 0, myCanvas.width, myCanvas.height);

    drawGrid(channel, name);
}

function drawGrid(channel, name) {
    var myCanvas = document.getElementById(name);
    var context = myCanvas.getContext("2d");

    // axis
    context.moveTo(0, 128.5);
    context.lineTo(257, 128.5);
    context.moveTo(128.5, 0);
    context.lineTo(128.5, 257);

    // borders
    context.moveTo(0.5, 0);
    context.lineTo(0.5, 257);
    context.moveTo(256.5, 0);
    context.lineTo(256.5, 257);

    context.moveTo(0, 0.5);
    context.lineTo(257, 0.5);
    context.moveTo(0, 256.5);
    context.lineTo(257, 256.5);

    context.strokeStyle = "#2B2";
    context.stroke();
}

function updateCharts(channel, name) {
    var samples;
    samples = getNewValue(channel);
    drawScatter(channel, name, samples);
}

function resetGraph(channel, name) {
    //samples = [];
    samplesCount[channel] = 0;
    initialDraw(channel, name);
}

function drawScatter(channel, name, samples) {
    if (samplesCount[channel] > 3000) {
        resetGraph(channel, name);
        getNewValue(channel);
    }
    else {
        plotSamples(channel, name, samples);
    }
}

function plotSamples(channel, name, samples) {
    var myCanvas = document.getElementById(name);
    var context = myCanvas.getContext("2d");

    for (var i = 0; i < samples.length; i++) {
        context.fillStyle = 'rgba(255,255,255,.5)';
        context.fillRect(samples[i][0], samples[i][1], 1, 1);
    }
}

function updateStatus(channel) 
{
    var mode, annex, i=0;
    var mediafrontendStatus = new BDlna_MediaFrontendStatus();
    $MediaPlayer.getFrontendStatus(channel, mediafrontendStatus, streamData);

    //for (var i = 0; i < mediafrontendStatus.numDS; i++) 
    //{
        //alert('Freq[' + i + '] = ' + mediafrontendStatus.streamData[i].frequency);
        //alert('receiverLock[' + i + '] = ' + mediafrontendStatus.streamData[i].receiverLock);
        //alert('fecLock[' + i + '] = ' + mediafrontendStatus.streamData[i].fecLock);
        //alert('annex[' + i + '] = ' + mediafrontendStatus.streamData[i].annex);
        //alert('symbolRate[' + i + '] = ' + mediafrontendStatus.streamData[i].symbolRate);
        //alert('snrEstimate[' + i + '] = ' + mediafrontendStatus.streamData[i].snrEstimate);
        //alert('dsChannelPower[' + i + '] = ' + mediafrontendStatus.streamData[i].dsChannelPower);

        if (mediafrontendStatus.streamData[i].receiverLock == 1) {
            document.getElementById('receiverlock0').innerHTML = "Lock";
            document.getElementById('receiverlock0').style.color = 'Green';
        }
        else {
            document.getElementById('receiverlock0').innerHTML = "Unlock";
            document.getElementById('receiverlock0').style.color = 'Red';
        }

        if (mediafrontendStatus.streamData[i].fecLock == 1) {
            document.getElementById('feclock0').innerHTML = "Lock";
            document.getElementById('feclock0').style.color = 'Green';
        }
        else {
            document.getElementById('feclock0').innerHTML = "Unlock";
            document.getElementById('feclock0').style.color = 'Red';
        }

        switch (mediafrontendStatus.streamData[i].mode)
	    {
		    case BMedia_FrontendMode_e16:
			    mode = "QAM16";
			    break;
		    case BMedia_FrontendMode_e32:
		        mode = "QAM32";
			    break;
		    case BMedia_FrontendMode_e64:
		        mode = "QAM64";
			    break;
		    case BMedia_FrontendMode_e128:
		        mode = "QAM128";
			    break;
		    case BMedia_FrontendMode_e256:
		        mode = "QAM256";
			    break;
		    case BMedia_FrontendMode_e512:
		        mode = "QAM512";
			    break;
		    case BMedia_FrontendMode_e1024:
		        mode = "QAM1024";
			    break;
		    case NEXUS_FrontendQamMode_e2048:
			    mode = "QAM2048";
			    break;
		    case BMedia_FrontendMode_e4096:
			    mode = "QAM4096";
			    break;
		    case BMedia_FrontendMode_eAuto_64_256:	 /* Automatically scan both QAM-64 and QAM-256.
														    Not available on all chipsets. */
		    default:
			    mode = "QAMAUTO";
			    break;
        }
	    switch (mediafrontendStatus.streamData[i].annex)
	    {
		    case BMedia_FrontendQamAnnex_eA:
                annex = "Annex A";
			    break;
		    case BMedia_FrontendQamAnnex_eB:
		    default:
		        annex = "Annex B";
		        break;
	    }
		document.getElementById('mode0').innerHTML = mode + " " + annex;

        document.getElementById('tunerfreq0').innerHTML = mediafrontendStatus.streamData[i].frequency + " Hz";
        document.getElementById('symbolrate0').innerHTML = mediafrontendStatus.streamData[i].symbolRate + " Baud";
        document.getElementById('snr0').innerHTML = (mediafrontendStatus.streamData[i].snrEstimate / 100.0) + " dB";
        // document.getElementById('power0').innerHTML = mediafrontendStatus.streamData[i].power + " dbm";
        document.getElementById('timeElapsed0').innerHTML = (mediafrontendStatus.streamData[i].timeElapsed/1000) + " secs";
    //}
}

function toggleconstInterval(channel, enable, name) {
    if (!enable)
    {
        if (interval[channel]) {
            //alert("clearing const interval on channel " + channel);
            clearInterval(interval[channel]);
            interval[channel] = false;
        }
    }
    else {
        if (!interval[channel]) {
            getNewValue(channel);
            initialDraw(channel, name);
            //alert("setting up const interval on channel " + channel);
            interval[channel] = setInterval("updateCharts(" + channel + ",\"" + name + "\")", delay);
        }
    }
}

function togglestatusInterval(channel, enable) 
{
    if (!enable) {
        if (intervalStatus[channel]) {
            //alert("clearing status interval on channel " + channel);
            clearInterval(intervalStatus[channel]);
            intervalStatus[channel] = false;
        }
    }
    else {
        if (!intervalStatus[channel]) {
            //alert("setting up status interval on channel " + channel);
            intervalStatus[channel] = setInterval("updateStatus(" + channel + ")", 2000);
        }
    }
}

