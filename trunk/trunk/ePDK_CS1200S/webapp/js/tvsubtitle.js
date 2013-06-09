//interface/remote controls for TV subtitle & audio

var _subtitles = null;
var _subtitlesIndex = 0;
var _audios = null;
var _audiosIndex = 1;
var _muted = 0;
var _volume = 0;

function GetSubTitle()
{
	tvSubTitleRpc();
}

function HideSubTitle()
{
	channelBoxHide();
	_channelBoxIntervalCount = 0;
		
	_channelBoxActive = false;
	_subtitleActive = false;
}

function ShowSubTitle(lang)
{
	var spacer = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	var promptStyle = "<span id=\"subtitlePrompt\" style='color:white; font-family:arial; font-weight:normal; font-size:medium'>";
	var promptImage = "<img id=\"subtitlePromptImg\" src=\"./images/updown-button.png\" ALIGN=ABSMIDDLE height=\"32px\" width=\"32px\" ></img>";
	var langStyle = "<span id=\"subtitleLang\" style='color:white; font-family:arial; font-weight:normal; font-size:medium'>";

	// dismiss current channelbox if any
	if (_channelBoxInterval != null)
	{
		clearInterval(_channelBoxInterval);
		_channelBoxInterval = null;
	}
	channelBoxHide();
	_channelBoxIntervalCount = 0;
		
	channelBoxShow(spacer + promptStyle + "Subtitle:&nbsp;&nbsp;</span>" + promptImage + langStyle + "&nbsp;" + lang + "</span>");
	_channelBoxActive = true;
	_subtitleActive = true;
}

function InitGetSubTitleList(myObject)
{
	var subtitleList = myObject.result;
	var length = subtitleList.length;
	
	_subtitles = new Array(length+1);
	_subtitles[0] = new Object();
	_subtitles[0]['text'] = 
		{ streamId: -1,
		  substreamId: -1, 
		  lang: "OFF",
		  additionalInfo: ""}; 
	for (var i = 0; i < length; i++) {
		var subtitle = subtitleList[i];
		_subtitles[i+1] = new Object();
		_subtitles[i+1]['text'] = 
			{ streamId: subtitle.streamId,
			  substreamId: subtitle.substreamId, 
			  lang: subtitle.languageDescription.iso639_1,
			  additionalInfo: subtitle.additionalInfo}; 
	}

	// for subtitle state use only:
	//tvSubTitleGetStreamRpc(false);
	
	var lang = _subtitlesIndex + " / " + (length) + "&nbsp;" + _subtitles[_subtitlesIndex]['text'].lang
		+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + _subtitles[_subtitlesIndex]['text'].additionalInfo;
	ShowSubTitle(lang);
}

function InitGetAudioVolume(myObject)
{
	_volume = myObject.result.value;// saved current volume level
	tvAudioSetVolumeRpc(0, false);	// then set to mute volume 0
	ShowMuteImage();
}

function InitGetAudioStream(myObject)
{
	_audiosIndex = myObject.result.value;// saved current audio stream
	_audiosIndex++;	 // we use offset starting from 1
	
	tvGetAudioAttributesRpc();
}

function ShowMuteImage()
{
	document.getElementById("tvaudio").className = "tvAudioImage";
}

function HideMuteImage()
{
	document.getElementById("tvaudio").className = "hidden";
}

function GetSubTitleStream(myObject)
{
	var subtitleList = myObject.result;
	var length = subtitleList.length;
	var streamIndex = subtitleList.value;
	alert('GetSubTitleStream, subtitleStreamIndex=' + streamIndex);
}

function MoveDownSubTitle()
{
	var length = _subtitles.length;
	_subtitlesIndex = (_subtitlesIndex+1) % length;
	var lang = _subtitlesIndex + " / " + (length-1) + "&nbsp;" + _subtitles[_subtitlesIndex]['text'].lang
		+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + _subtitles[_subtitlesIndex]['text'].additionalInfo;
	ShowSubTitle(lang);
	
	if (_subtitlesIndex > 0)
	{
		tvSubTitleSetVisiblityRpc(true, false);
		tvSubTitleSetStreamRpc(_subtitlesIndex-1, false);
	}
	else
		tvSubTitleSetVisiblityRpc(false, false);
}

function MoveUpSubTitle()
{
	var length = _subtitles.length;
	if (_subtitlesIndex > 0)
		_subtitlesIndex--;
	else
		_subtitlesIndex = length-1;
	var lang = _subtitlesIndex + " / " + (length-1) + "&nbsp;" + _subtitles[_subtitlesIndex]['text'].lang
		+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + _subtitles[_subtitlesIndex]['text'].additionalInfo;
	ShowSubTitle(lang);

	if (_subtitlesIndex > 0)
	{
		tvSubTitleSetVisiblityRpc(true, false);
		tvSubTitleSetStreamRpc(_subtitlesIndex-1, false);
	}
	else
		tvSubTitleSetVisiblityRpc(false, false);
}

function GetAudioAttributes()
{
	tvGetAudioStreamRpc(false);
}

function HideAudio()
{
	if (_audioActive)
	{
		channelBoxHide();
		_channelBoxIntervalCount = 0;
			
		_channelBoxActive = false;
		_audioActive = false;
	}
}

function InitGetAudioList(myObject)
{
	var audioList = myObject.result;
	var length = audioList.length;
	
	_audios = new Array(length+1);
	_audios[0] = new Object();
	_audios[0]['text'] = 
		{ streamId: -1,
		  substreamId: -1, 
		  lang: "OFF",
		  additionalInfo: ""}; 
	for (var i = 0; i < length; i++) {
		var audio = audioList[i];
		_audios[i+1] = new Object();
		_audios[i+1]['text'] = 
			{ streamId: audio.streamId,
			  substreamId: audio.substreamId, 
			  lang: audio.languageDescription.iso639_1,
			  additionalInfo: audio.additionalInfo}; 
	}

	// for audio state use only:
	//tvSubTitleGetAudioStreamRpc(false);
	
	var lang = _audiosIndex + " / " + (length) + "&nbsp;" + _audios[_audiosIndex]['text'].lang
		+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + _audios[_audiosIndex]['text'].additionalInfo;
	ShowAudioList(lang);
}

function ShowAudioList(lang)
{
	var spacer = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	var promptStyle = "<span id=\"subtitlePrompt\" style='color:white; font-family:arial; font-weight:normal; font-size:medium'>";
	var promptImage = "<img id=\"subtitlePromptImg\" src=\"./images/updown-button.png\" ALIGN=ABSMIDDLE height=\"32px\" width=\"32px\" ></img>";
	var langStyle = "<span id=\"subtitleLang\" style='color:white; font-family:arial; font-weight:normal; font-size:medium'>";

	// dismiss current channelbox if any
	if (_channelBoxInterval != null)
	{
		clearInterval(_channelBoxInterval);
		_channelBoxInterval = null;
	}
	channelBoxHide();
	_channelBoxIntervalCount = 0;
		
	channelBoxShow(spacer + promptStyle + "Audio:&nbsp;&nbsp;</span>" + promptImage + langStyle + "&nbsp;" + lang + "</span>");
	_channelBoxActive = true;
	_audioActive = true;
}

function MoveDownAudio()
{
	var length = _audios.length;
	_audiosIndex = (_audiosIndex+1) % length;
	if (_audiosIndex == 0)
		_audiosIndex++;
	var lang = _audiosIndex + " / " + (length-1) + "&nbsp;" + _audios[_audiosIndex]['text'].lang
		+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + _audios[_audiosIndex]['text'].additionalInfo;
	ShowAudioList(lang);
	
	tvAudioSetStreamRpc(_audiosIndex-1, false);
}

function MoveUpAudio()
{
	var length = _audios.length;
	if (_audiosIndex > 0)
		_audiosIndex--;
	if (_audiosIndex == 0)
		_audiosIndex = length-1;
	var lang = _audiosIndex + " / " + (length-1) + "&nbsp;" + _audios[_audiosIndex]['text'].lang
		+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + _audios[_audiosIndex]['text'].additionalInfo;
	ShowAudioList(lang);

	tvAudioSetStreamRpc(_audiosIndex-1, false);
}

function MuteAudio()
{
	if (_muted)
		_muted = 0;
	else
		_muted = 1;
	
	if (_muted)
		tvAudioVolumeRpc(false);
	else
	{
		tvAudioSetVolumeRpc(_volume, false);
		HideMuteImage();
	}
}



