// 1)extract TV guide data and insert into _channels[]['channel'], _channels[]['programs'] & _headers
// 2)setup _headerStart, _headerEnd, _channelStart, _channelEnd
function GetEPG(myObject)
{
	var len = myObject.result.length;
	if (myObject.id < 100)	// channel idx is offset by 100
		return;
	var channelIdx = myObject.id - 100;
	//alert('len=' + len + ', id=' + myObject.id + 'channelIdx=' + channelIdx);

	//alert(myObject.result[0]); //- XML format

	//if (channelIdx >= 1)	// shorter list test
	//{
	//	alert('skipping channelIdx = ' + channelIdx);
	//	break;
	//}
	
	_channels[channelIdx]['programs'] = new Array();
	for( var i = 0; i < len; i++) {
		//alert(myObject.result[i].title); 
		
	    var serviceId = myObject.result[i].serviceId;
		var starttime = myObject.result[i].startTime * 1000;
		var endtime = myObject.result[i].endTime * 1000;
		var title = myObject.result[i].title;
		var desc = myObject.result[i].description;

		var utcStart = new Date(starttime);
		var utcEnd = new Date(endtime);
		
		if (_utcStart != null)
		{
			var utcDiff = utcStart.getTime() - _utcStart.getTime(); 
			var spanDays = Math.ceil( utcDiff / (1000*60*60*24) );	// span in days
			if (spanDays > _maxEPGDay)
			{
				//alert('--------- breaking out of max ' + _maxEPGDay + ' day schedule'
				//	+ ', channelIdx=' + channelIdx
				//	+ ', spanDays=' + spanDays
				//	+ ', utcStart=' + utcStart.toUTCString()
				//	+ ', _utcStart=' + _utcStart.toUTCString());
				break;
			}
		}

		var span = endtime - starttime;
		var spanMins = Math.ceil( span / (1000*60) );	// span in mins
		//alert('-------i=' + i + ', channelIdx=' + channelIdx 
		//	+ ', spanMins=' + spanMins 
		//	+ ' , utcStart=' + utcStart.toUTCString()
		//	+ ' , utcEnd=' + utcEnd.toUTCString());
		if (spanMins < 0)
		{
			//alert('-------------bad spanMins=' + spanMins + ', channelIdx=' + channelIdx + ', i=' + i 
			//	+ ' , utcStart=' + utcStart.toUTCString() + ', utcEnd=' + utcEnd.toUTCString());
			break;
		}
		
		_channels[channelIdx]['programs'][i] = new Program(String(serviceId), title, desc, starttime, endtime, span);
		//if (channelIdx == 2)
		//{
		//	var utcStart = new Date(starttime);
		//	var utcEnd = new Date(endtime);
		//	var spanMins = Math.ceil( span / (1000*60) );	// span in mins
		//	alert('==============' + _channels[channelIdx]['programs'][i].title
		//		+ ', spanMins=' + spanMins 
		//		+ ' , utcStart=' + utcStart.toUTCString()
		//		+ ' , utcEnd=' + utcEnd.toUTCString()
		//	);
		//}
		
		if ((channelIdx == 0) && (i == 0))
			_utcStart = utcStart;
	}

	// headers setup is based on _currentTime
	if (_headers == null)
		SetupHeaders();
}

function SetupHeaders()
{
	var curTime = new Date(_currentTime*1000);
	var intervalSpan = 30;	// 30 minutes span
	var curTimeHrs = curTime.getUTCHours();
	var curTimeMins = curTime.getUTCMinutes();
	var noCells = (_maxEPGDay * 24) * 2;	// 24 hour period cycle

	curTime.setSeconds(0);
	if (curTimeMins >= 30)
	{
		curTime.setMinutes(30);
		noCells--;
	}
	else
		curTime.setMinutes(0);
	
	_headers = new Array(noCells);
	for (var i = 0; i < _headers.length; i++) 
	{
		var hrs = curTime.getUTCHours();
		var mins = curTime.getUTCMinutes();
		var fTime = friendlyTime(hrs, mins);
		
		var start = curTime.getTime();
		var end = start + (intervalSpan * 60 * 1000);
		var span = end - start;
		var spanMins = Math.ceil( span / (1000*60) );	// span in mins
		
		//alert('++++++++friendlyTime=' + fTime 
		//	+ ', start=' + start + ', utcStart=' + curTime.toUTCString()
		//	+ ', end=' + end + ', utcEnd=' + new Date(end).toUTCString()
		//	+ ', spanMins=' + spanMins);
		
		_headers[i] = new Header(fTime, start, end, span);
		curTime.setTime(end);
	}
}

function friendlyTime(hour, minute) 
{
   var ap = "AM";
   if (hour > 11) { ap = "PM"; }
   if (hour > 12) { hour = hour - 12;}
   if (hour == 0) { hour = 12;}
   if (hour < 10) { hour   = "0" + hour;}
   if (minute < 10) { minute = "0" + minute;}
   var timeString = hour + ':' + minute + " " + ap;
   return timeString;
}

