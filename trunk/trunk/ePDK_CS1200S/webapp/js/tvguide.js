	// <body bgcolor="#33ccff">
	var _monitorShowTimeInterval = 60;	// timestamp every 60 secs
	var _useVideoTag = false;
	var _useMongoose = true;
	var _pipX = 109;
	var _pipY = 47;
	var _pipWidth = 303;
	var _pipHeight = 208;
	var _maxChannels = 20;
	var _keydownEvent = null;
	var _touchStartEvent = null;
	var _touchMoveEvent = null;
	var _touchEndEvent = null;
	var _touchCancelEvent = null;
	var _headerwidth = 165;
    var _pixperspan = ((1065 - _headerwidth) / 5) / 30;
    var _programBkColor = "#26354A";
    var _channels = null;
    var _headers = null;
    var _headerStart = 0;
    var _headerEnd = 4;
    var _programStart = 0;
    var _programEnd = 4;
	var _maxChannelEnd = 7;
    var _channelStart = 0;
    var _channelEnd = _maxChannelEnd;
    var _rIdx = -1;
    var _cIdx = -1;
    var _rMaxIdx = -1;
    var _oObj = null;
	var _init = false;
	var _timeBar = false;
	var _curTime = null;
	var _currentTime = null;
	var _utcStart = null;
	var _showTimeInterval = null;
	var _tvAvail = true;
	var _lookAheadPgm = true;
	var _isTV = false;
	var _debug = true;
	var _msg = "";
	var _maxEPGDay = 2;
	
	function InitGuideList(myObject) {
		var channelList = myObject.result;
		var noChannels = channelList.length;
		if (noChannels == 0)
			return;
		_channels = new Array(noChannels);
		_channelStart = 0;
		if (noChannels <= _maxChannelEnd)
			_channelEnd	= noChannels-1;
		else
			_channelEnd	= _maxChannelEnd;	// max at 5 channels

		for (var i = 0; i < channelList.length; i++) {
			var channel = channelList[i];
			_channels[i] = new Object();
			_channels[i]['channel'] = 
				{ name: channel.name,
				  number: channel.channelNumber, 
				  id: channel.id, 
				  img: "",
				  uri: channel.uri};
		}
		
		for (var i = 0; i < _channels.length; i++)
		{
			if (_channels[i]['channel'].name == "BBC ONE")
				_channels[i]['channel'].img = "<img src=\"images/BBCOne.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC TWO")
				_channels[i]['channel'].img = "<img src=\"images/BBCTwo.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC THREE")
				_channels[i]['channel'].img = "<img src=\"images/BBCThree.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC FOUR")
				_channels[i]['channel'].img = "<img src=\"images/BBCFour.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "ITV1")
				_channels[i]['channel'].img = "<img src=\"images/ITV1.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "Channel 4")
				_channels[i]['channel'].img = "<img src=\"images/Channel4.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "Channel 5")
				_channels[i]['channel'].img = "<img src=\"images/Channel5.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "CBeebies")
				_channels[i]['channel'].img = "<img src=\"images/CBeebies.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC Parliament")
				_channels[i]['channel'].img = "<img src=\"images/BBCParliament.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC NEWS")
				_channels[i]['channel'].img = "<img src=\"images/BBCNews.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC One HD")
				_channels[i]['channel'].img = "<img src=\"images/BBCOne.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "Channel 4 HD")
				_channels[i]['channel'].img = "<img src=\"images/Channel4HD.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "ITV1 HD")
				_channels[i]['channel'].img = "<img src=\"images/ITV1.jpg\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "BBC HD")
				_channels[i]['channel'].img = "<img src=\"images/BBCHD.png\" height=\"16\" width=\"30\">";
			else if (_channels[i]['channel'].name == "301")
				_channels[i]['channel'].img = "<img src=\"images/BBC301.png\" height=\"16\" width=\"30\">";
		}

		tvGuideQueryRpc(_currentTime);
	}

	function InitCurrentTime(myObject)
	{
		var secs = myObject.result.value;
		var gmtTime = new Date(secs * 1000);
		_currentTime = secs;
		_headers = null;	// reset headers
		_headerStart = 0;
		_headerEnd = 4
		_rIdx = -1;
		//if (_debug)
		//	Debug('Debug: Current time=' + secs + ' - ' + gmtTime.toUTCString());
		//Debug2("2nd debug goes here->");
	}

	function AddGuideEPG(myObject) {
		// debug messages
		//var channelIdx1 = myObject.id - 100;
		//_msg = _msg + ", " + channelIdx1;
		//Debug2(_msg);
		
		GetEPG(myObject);
		
		// start showing guide as soon as we get first _maxChannelEnd+1 channels
		var len = myObject.result.length;
		if (myObject.id < 100)	// channel idx is offset by 100
			return;
		var channelIdx = myObject.id - 100;
		tvGuideQueryRpcStages(channelIdx + 1);
		var len = _channels.length;
		//alert('++++++++++after GetEPG, channelIdx=' + channelIdx + ' ,len=' + len + ', _maxChannelEnd=' + _maxChannelEnd);
		if ((len < (_maxChannelEnd+1)) && (channelIdx == (len-1)))
			StartShowingGuide();
		else if (channelIdx == _maxChannelEnd)
			StartShowingGuide();
	}

	function StartShowingGuide()
	{
		showGuide();
		if (_channelGuideActive)
		{
			channelBoxHide();
			Debug("");
			stopMonitorGuideLoad();
			if (_tvAvail)
				StartTV();
		}
		StartShowTime();
	}
	
	function StartShowTime()
	{
		if (_showTimeInterval == null)
		{
			monitorShowTime();
			_showTimeInterval = setInterval("monitorShowTime()", _monitorShowTimeInterval*1000);
		}
	}
	
	function StopShowTime()
	{
		if (_showTimeInterval)
		{
			clearInterval(_showTimeInterval);
			_showTimeInterval = null;
		}
	}
	
	function monitorShowTime()
	{
		tvUTCTimeGetMonitor();
	}
	
	function ShowCurrentTime(myObject)
	{
		var el = document.getElementById("tvDetailCurrentTime");
		var msecs = myObject.result.value * 1000;
		var gmtTime = new Date(msecs);

		el.className = "tvDetailCurrentTime";
		var ftime = friendlyTime(gmtTime.getUTCHours(), gmtTime.getUTCMinutes());
		el.innerHTML = ftime;

		//Debug('Current time=' + msecs + ' - ' + gmtTime.toUTCString());
	}
	
    function LoadGuide() {
		if (!_init)
		{
			if (!_wsAvail)
				GetEPGDummy();
			GetEventObj();
			_keydownEvent = function (event) { keyCheck(event); };
			_oObj.addEvent(document, "keydown", _keydownEvent);

			if (_isIPad)
			{
				_touchStartEvent = function (event) { touchStart(event, 'guideTable'); };
				_oObj.addEvent(document, "touchstart", _touchStartEvent);
				
				_touchMoveEvent = function (event) { touchMove(event); };
				_oObj.addEvent(document, "touchmove", _touchMoveEvent);
				
				_touchEndEvent = function (event) { touchEnd(event); };
				_oObj.addEvent(document, "touchend", _touchEndEvent);
				
				_touchCancelEvent = function (event) { touchCancel(event); };
				_oObj.addEvent(document, "touchcancel", _touchCancelEvent);
			}
			
			_init = true;
		}
		showDetail(null);
		RefreshGuide();
    }
	
    function RefreshGuide() {
		RemoveGuide();
        AddGuide();
    }

	function AddGuide() {
        AddHeader();
        if (_timeBar)
			AddTimeBar();
        AddProgram();
	}

	function StartTV()
	{
        var defCell = document.getElementById("cell_" + _rIdx + "_" + _cIdx);
        if (defCell) {
			var uri = defCell['Uri'];
			if (uri != "")
			{
				//Debug2('start tv on rIdx=' + _rIdx + ',selectedIndex=' + defCell['Idx'] + ', uri=' + uri + ', chName=' + defCell['Name']);
				tvStartRpc(uri);
				var i = defCell['Idx'];
				if ((i != null) && (i>=0))
					_selectedChannelIndex = i;
			}
		}
	}
	
    function AddTimeBar() {
        var oCell, oRow;
        var oTable = document.createElement("TABLE");
        var oTHead = document.createElement("THEAD");

        // create spacer
        oRow = document.createElement("TR");
        oTHead.appendChild(oRow);
		
        oCell = document.createElement("TD");
        oRow.appendChild(oCell);
        
        // table border attrs
        oTable.border = 1;
		oTable.style.borderColor = "green";	
		oCell.style.borderTop = "none";
		oCell.style.borderBottom = "none";
		oTable.borderBottom = '0px';
        oTable.cellpadding = 1;
        oTable.cellSpacing = 0;
        oTable.style.width = "100%";
        oTable.style.tableLayout = "fixed";

		var container = document.getElementById("guideTable");
        container.appendChild(oTable);
	}

    function AddHeader() {
        var oRow, oCell, i, j, start, utcStart;
        oTable = document.createElement("TABLE");
        var oTHead = document.createElement("THEAD");

        // table border attrs
        oTable.border = 1;
		oTable.style.borderColor = "black";	
		//oTable.borderBottom = '0px';
        //oTable.bgColor = _programBkColor;
        oTable.cellpadding = 1;
        oTable.cellSpacing = 0;
        oTable.style.width = "100%";
        oTable.style.tableLayout = "fixed";
        
        var container = document.getElementById("guideTable");
        container.appendChild(oTable);
        oTable.appendChild(oTHead);

        // insert a row into header
        oRow = document.createElement("TR");
		oRow.style.height = '35px';
        oTHead.appendChild(oRow);
        //oTHead.setAttribute("bgColor", "#424242");
        oTHead.setAttribute("align", "left");
        oTHead.setAttribute("fontWeight", "normal");

        // insert cells into the header row
        for (i = _headerStart, j=0; i <= _headerEnd+1; i++, j++) {
            oCell = document.createElement("TH");
            //oCell.style.fontSize = 'small';
            //oCell.style.fontWeight = 'normal';
            //oCell.align = 'center';
			oCell.className = 'tvHeaderCell';
            if (j == 0) {
				if (_utcStart != null)
				{
					oCell.style.fontWeight = 'normal';
					oCell.style.paddingLeft = "6px";
					if (i <= _headers.length)
					{
						start = _headers[i].start;
						utcStart = new Date(start);
						oCell.innerHTML = utcStart.toDateString();
						oCell.id = "header_0_0";
					}
				}
				else
					oCell.innerHTML = '&nbsp;';
                oCell.style.width = String(_headerwidth) + 'px';
                //oCell.style.backgroundColor = _programBkColor;
            }
            else {
				if (i > _headers.length)
				{
					break;
				}
                oCell.innerHTML = _headers[i-1].title;
                oCell.style.width = _pixperspan * 30;
            }
            oRow.appendChild(oCell);
        }
    }

    function GetEventObj() {
		if (_oObj != null) return;
        _oObj = {
            addEvent: function (obj, evtype, func) {
                try {
                    if (window.attachEvent) {
                        obj.attachEvent("on" + evtype, func);
                    }
                    if (window.addEventListener) {
                        obj.addEventListener(evtype, func, false);
                    }
                } catch (e) { }
            },
            removeEvent: function (obj, evtype, func) {
                try {
                    if (window.detachEvent) {
                        obj.detachEvent("on" + evtype, func);
                    }
                    if (window.removeEventListener) {
                        obj.removeEventListener(evtype, func, false);
                    }
                } catch (e) { }
            }
        };
    }

	function ChangeTableBkColor(clr)
	{
		var oTable = document.getElementById("guideTable");
		if (oTable != null)
		{
			if (clr == 'A')
				oTable.bgColor = "#26354A";
			else if (clr == 'B')
				oTable.bgColor = "#663333";
			else if (clr == 'C')
				oTable.bgColor = "#244766";
			else if (clr == 'D')
				oTable.bgColor = "#26354A";
		}
	}
	
    function AddProgram() {
        var r, c;

        // insert channels & programs
        for (var i = _channelStart, r=0; i <= _channelEnd; i++, r++) {
            var oTable = document.createElement("TABLE");
            var container = document.getElementById("guideTable");
            var oTBody0 = document.createElement("TBODY");
            var oBody = oTBody0;
            var oCellCount = 0;
            container.appendChild(oTable);

            // table border attrs
            //oTable.style.borderColor = "salmon";
            oTable.border = 1;  
            oTable.cellPadding = 0;
            oTable.cellSpacing = 0;
            oTable.style.tableLayout = "fixed";
            oTable.style.width = "100%";
            //oTable.bgColor = _programBkColor;
			oTable.id = "guideTbl";

            oRow = document.createElement("TR");
            oBody.appendChild(oRow);

            oCellCount = AddChannel(r, i, oCellCount, oRow);
			oCellCount = AddChannelProgram(r, i, oCellCount, oRow);
            oTable.appendChild(oTBody0);
        }
		
        if (_rIdx == -1) {
            _rIdx = 0;  // default cell selected indexes
			if (_selectedChannelIndex != -1)
				_rIdx = _selectedChannelIndex;
            _cIdx = 1;
			SetFocus();
			enterKey();
			return;
        }
        SetFocus();
    }
    
    function AddChannel(r, i, oCellCount, oRow)
    {
        var oCell = document.createElement("TD");
        AssignCellId(oCell, r, oCellCount);
        oCellCount++;

        var txt = _channels[i]['channel'].img + '&nbsp;' + _channels[i]['channel'].name;
		var desc = _channels[i]['channel'].desc;
		var uri = _channels[i]['channel'].uri;
		var name = _channels[i]['channel'].name;
        oCell = AssignCellProp(oCell, txt, "-1", "-1", name, desc, uri, i);
		oCell.className = 'tvChannelCell';
		
		oCell.style.borderLeft = "solid black 1px";	// Mike
		oCell.style.borderTop = "none";	// Mike
		oCell.style.borderBottom = "none";	// Mike
		oCell.style.borderRight = "solid black 1px";	// Mike
		
        oCell.style.width = String(_headerwidth) + 'px';
        oRow.appendChild(oCell);
        return oCellCount;
    }

    function AddChannelProgram(r, i, oCellCount, oRow) 
	{
        var difference, p, c, length, name, desc, title, len, uri, lengthMins;
        var startIdx = -1, endIdx = -1;
		var headerStart, headerEnd, headerSpan;

        // setup startIdx, endIdx
        headerStart = _headers[_headerStart].start;
		if (_headerEnd < _headers.length)	// ensure we have enough header cells
			headerEnd = _headers[_headerEnd].end;
		else
			headerEnd = _headers[_headers.length-1].end;
        headerSpan = headerEnd - headerStart;

		if (_channels[i]['programs'] != null)
			len = _channels[i]['programs'].length;
		else 
			len = 0;
			
		//alert('++++++++++++++++++++++AddChannelProgram+++++++++++++++++++, r=' + r + ', len=' + len 
		//	+ ', _headers.length=' 	+ _headers.length 
		//	+ ', headerStart=' + headerStart 
		//	+ ', headerEnd=' + headerEnd);
        for (p = 0; p < len; p++) {
			if (_channels[i]['programs'][p] == null)
			{
				break;
			}
			else
			{
				//alert('p=' + p + ', title=' + _channels[i]['programs'][p].title);
			}
            if (_channels[i]['programs'][p].start >= headerStart) {
                startIdx = p;
                break;
            }
        }
		
        if (startIdx != -1) {
            p = startIdx;
            while ((p < len) && (_channels[i]['programs'][p] != null) && (_channels[i]['programs'][p].end <= headerEnd)) {
                p++;
            }
            if (p == startIdx)
                endIdx = startIdx;
            else
                endIdx = p - 1;
        }
		
		//alert('++++++++++++++++++++++++++++startIdx=' + startIdx + ', endIdx=' + endIdx);	
        var totalSpan = 0;
        if (startIdx == -1) // case 1: no pgm
        {
            length = headerEnd - headerStart;
			if ((totalSpan + length) > headerSpan)
				length = headerSpan - totalSpan;
			start = end = -1;
            title = 'Offline';
			desc = "No information available";
			uri = _channels[i]['channel'].uri;  // default to current channel uri
			name = _channels[i]['channel'].name;
			if (_lookAheadPgm)
			{
				var idx = PgmReverseEnd(i, p-1, headerStart, AddSpan(headerStart, length) );
				if (idx != -1)
				{
					title = "< " + _channels[i]['programs'][idx].title;
					desc = _channels[i]['programs'][idx].desc;
					start = _channels[i]['programs'][idx].start;
					end = _channels[i]['programs'][idx].end;
				}
			}
		
            oCellCount = AddCell(r, i, oCellCount, title, length, start, end, name, desc, uri);
            return oCellCount;
        }

        // start adding program
        for (p = startIdx, c=0; p <= endIdx; p++, c++) {
            // case 2: prefix empty pgm
            var lastStart = totalSpan + _headers[_headerStart].start;
            var nextStart = _channels[i]['programs'][p].start;
			var start = "-1", end = "-1";
			length = nextStart - lastStart;
			if ((nextStart > lastStart) && (length != headerSpan))
			{
				if ((totalSpan + length) > headerSpan)
				{
					length = headerSpan - totalSpan;
					if (c == 0)
					{
						title = "< ";	// 1st cell
						desc = "No information available";
						if (_lookAheadPgm)
						{
							if (length == headerSpan)	// if totally empty, get pgm if any
							{
								var idx = PgmReverse(i, p, headerStart, headerEnd);
								if (idx != -1)
								{
									title = '< ' + _channels[i]['programs'][idx].title;	// 1st cell
									desc = _channels[i]['programs'][idx].desc; 
									start = _channels[i]['programs'][idx].start;
									end = _channels[i]['programs'][idx].end;
								}
							}
						}
					}
					else
					{
						title = " >";	// only cell
						desc = "No information available";
					}
					uri = _channels[i]['channel'].uri;  // default to current channel uri
					name = _channels[i]['channel'].name;
					oCellCount = AddCell(r, i, oCellCount, title, length, start, end, name, desc, uri);
					totalSpan += length;
					break;
				}
				else
				{
					if (c == 0)
					{
						//title = '< Mike' + 'len=' + length + ', t=' + _channels[i]['programs'][p].title;	// 1st cell
						title = '< ';	// 1st cell
						desc = "No information available";
						if (_lookAheadPgm)
						{
							var idx = PgmReverseEnd(i, p-1, headerStart, AddSpan(headerStart, length) );
							if (idx != -1)
							{
								title = "< " + _channels[i]['programs'][idx].title;
								desc = _channels[i]['programs'][idx].desc;
								start = _channels[i]['programs'][idx].start;
								end = _channels[i]['programs'][idx].end;
							}
						}
					}
					else
					{
						title = '&nbsp;';	// middle cell
						desc = 'No information available - middle cell';
					}
					uri = _channels[i]['channel'].uri;  // default to current channel uri
					name = _channels[i]['channel'].name;
					oCellCount = AddCell(r, i, oCellCount, title, length, start, end, name, desc, uri);
					totalSpan += length;
				}
            }

            // case 3: normal pgm
            length = _channels[i]['programs'][p].span;
			lengthMins = Math.ceil( length / (1000*60) );	// mins
			desc = _channels[i]['programs'][p].desc + " " + lengthMins + " mins";
			title = _channels[i]['programs'][p].title;
			uri = _channels[i]['channel'].uri;
			name = _channels[i]['channel'].name;
            if (totalSpan+length > headerSpan)	 // span exceeded remaining avail cell length
			{
				length = headerSpan - totalSpan;
				title = " >";	// last cell
				desc = "No information available";
				uri = "";
				if (_lookAheadPgm)
				{
					nextStart = AddSpan(headerStart, totalSpan);
					var idx = PgmAhead(i, p, nextStart, headerEnd, len);
					if (idx != -1)
					{
						title = _channels[i]['programs'][idx].title + " >";	// last cell
						desc = _channels[i]['programs'][idx].desc;
						uri = _channels[i]['channel'].uri;
						name = _channels[i]['channel'].name;
						start = _channels[i]['programs'][idx].start;
						end = _channels[i]['programs'][idx].end;
					}
				}
			}
            oCellCount = AddCell(r, i, oCellCount, title, length, _channels[i]['programs'][p].start, _channels[i]['programs'][p].end, name, desc, uri);
            totalSpan += length;
        }
                
        // case 4: expand last empty cell if needed
        if (totalSpan < headerSpan) {
			nextStart = AddSpan(headerStart, totalSpan);
            length =  headerSpan - totalSpan;
			title = " >";
			desc = "No information available";
			uri = _channels[i]['channel'].uri;  // default to current channel uri
			name = _channels[i]['channel'].name;
			start = nextStart;
			end = headerEnd;
			if (_lookAheadPgm)
			{
				var idx = PgmAhead(i, p, nextStart, headerEnd, len);
				if (idx != -1)
				{
					title = _channels[i]['programs'][idx].title + " >";	// last cell
					desc = _channels[i]['programs'][idx].desc;
					uri = _channels[i]['channel'].uri;
					name = _channels[i]['channel'].name;
					start = _channels[i]['programs'][idx].start;
					end = _channels[i]['programs'][idx].end;
				}
			}

            oCellCount = AddCell(r, i, oCellCount, title, length, start, end, name, desc, uri);
        }
    }

	function PgmReverse(r, p, headerStart, headerEnd)
	{
		for (var i=p; i>=0; i--)
		{
			if (_channels[r]['programs'][i] == null)
				break;
			if ((headerStart >= _channels[r]['programs'][i].start) && 
				(headerEnd <= _channels[r]['programs'][i].end))
				return i;
		}
		return -1;
	}
	
	function PgmReverseEnd(r, p, start, end)
	{
		for (var i=p; i>=0; i--)
		{
			if (_channels[r]['programs'][i] == null)
				break;
			if ((_channels[r]['programs'][i].end >= start) &&
				(_channels[r]['programs'][i].end <= end))
				return i;
		}
		return -1;
	}
	
	function PgmAhead(r, p, start, end, endIdx)
	{
		for (var i=p, c=0; i<=endIdx; i++, c++)
		{
			if (c > 3)	// abandon if not found by 3
				break;
			if (_channels[r]['programs'][i] == null)
				break;
			if ((_channels[r]['programs'][i].start <= end) &&
				(_channels[r]['programs'][i].start >= start))
				return i;
		}
		return -1;
	}

    function AddCell(r, i, oCellCount, txt, length, start, end, name, desc, uri) {
		var lengthMins;
        var oCell = document.createElement("TD");
        AssignCellId(oCell, r, oCellCount);
        oCellCount++;

		oCell.style.borderLeft = "solid black 1px";	// Mike
		oCell.style.borderTop = "none";	// Mike
		oCell.style.borderBottom = "none";	// Mike
		oCell.style.borderRight = "solid black 1px";	// Mike
		
		lengthMins = Math.ceil( length / (1000*60) );	// mins
        oCell.style.width = String(_pixperspan * lengthMins) + 'px';
        oCell = AssignCellProp(oCell, txt, start, end, name, desc, uri, i);
        oRow.appendChild(oCell);
        return oCellCount;
    }

    function AssignCellId(oCell, r, c) {
        oCell.tabIndex =-1;
        oCell.id = "cell_" + r + "_" + c;
        _rMaxIdx = r;
        _channels[r]['cellCount'] = c;
       _oObj.addEvent(oCell, "click", function() { OnClick(oCell);});
       _oObj.addEvent(oCell, "focus", function() { OnFocus(oCell);});
    }
	
	function OnFocus(oCell) {
		showDetail(oCell);
	}

    function AssignCellProp(oCell, txt, start, end, name, desc, uri, i) {
		oCell.className = 'tvCell';

        oCell.innerHTML = txt;

        oCell['StartTime'] = start;
        oCell['EndTime'] = end;
        oCell['Title'] = txt;
		oCell['Desc'] = desc;
		oCell['Uri'] = uri;
		oCell['Name'] = name;
		oCell['Idx'] = i;
		//alert('assign Uri=' + uri + ', txt=' + txt);
        return oCell;
    }

    function SetFocus() {
        var defCell = document.getElementById("cell_" + _rIdx + "_" + _cIdx);
        
        if (defCell) 
		{
			var headerTimeCell = document.getElementById("header_0_0");
			if (headerTimeCell)	// update header time cell
			{
				var starttime = defCell['StartTime'];
				if ((starttime) && (starttime != -1))
				{
					var utcStart = new Date(starttime);
					headerTimeCell.innerHTML = utcStart.toDateString();
				}
			}
		
            defCell.focus();
            OnFocus(defCell);
            //('Title=' + defCell.Title + ', Start=' + defCell.StartTime + ', End=' + defCell.EndTime + ', cell_' + _rIdx + '_' + _cIdx);
            return true;
        }
        else 
		{
            return false;
        }
    }

    function OnClick(oCell) {
        //Debug('OnClick, Title=' + oCell.Title + ', ' + oCell.id);
        var id = oCell.id.split("_");
        if (id.length == 3) {
            _rIdx = parseInt(id[1]);
            _cIdx = parseInt(id[2]);
        }
    }

    function Program(id, title, desc, start, end, span) {
        this.id = id;
        this.title = title;
		this.desc = desc;
        this.start = start;
        this.end = end;
        this.span = span;
    }

    function Header(title, start, end, span) {
        this.title = title;
        this.start = start;
        this.end = end;
        this.span = span;
    }

    function keyCheck(e) {
        try {
            var event = window.event || e;
            var keyID = event.charCode ? event.charCode : event.keyCode ? event.keyCode : 0;
			//if (_debug)
			//	Debug(keyID);
            switch (keyID) {
                case VK_OK:
                    enterKey();
                    break;
                case VK_LEFT:
                    event.returnValue = false;
                    moveLeft();
                    break;
                case VK_UP:
                    event.returnValue = false;
                    moveUp();
                    break;
                case VK_RIGHT:
                    event.returnValue = false;
                    moveRight();
                    break;
                case VK_DOWN:
                    event.returnValue = false;
                    moveDown();
                    break;
				case 33:	// keyboard PAGEUP
				case VK_CHANNEL_UP:
                    event.returnValue = false;
                    moveUp();
                    enterKey();
					if (_channelGuideActive)
					{
						if (_tvAvail)
							StartTV();
					}
					break;
				case 34:	// keyboard PAGEDOWN
				case VK_CHANNEL_DOWN:
                    event.returnValue = false;
                    moveDown();
                    enterKey();
					if (_channelGuideActive)
					{
						if (_tvAvail)
							StartTV();
					}
					break;
                case 36:    // keyboard home 
					break;
            }
        } catch (Exception) {
            return false;
        }
        return true;
    }

    function enterKey() {
        var cell = document.getElementById("cell_" + _rIdx + "_" + _cIdx);
        if (cell) {
            cell.focus();
            OnClick(cell);
			showChannelInfo(cell['Name']);
        }        
    }

    function moveRight() {
        var cCellCount = _channels[_rIdx]['cellCount'];
        if (_cIdx < cCellCount) {
            _cIdx++;
            SetFocus();
        } 
        else if (_cIdx == cCellCount)
        {
            scrollRight();
            _cIdx = _channels[_rIdx]['cellCount'];
            SetFocus();
        }
    }

    function moveLeft() {
        if (_cIdx > 1) {
            _cIdx--;
            SetFocus();
        } else if (_cIdx == 1)
            scrollLeft();
    }

    function moveUp() {
        if (_rIdx >= 1) {
            _rIdx--;
            if (!SetFocus()) {
                _cIdx = 1;  // default to 1st cell
                SetFocus();
            }
        } else if (_rIdx == 0)
            scrollUp();
    }

    function moveDown() {
        if (_rIdx < _rMaxIdx) {
            _rIdx++;
            if (!SetFocus()) {
                _cIdx = 1;  // default to 1st cell
                SetFocus();
            }
        } else if (_rIdx == _rMaxIdx)
            scrollDown();
    }

    function scrollRight() {
        if (_headerEnd < _headers.length - 2) {
            _headerStart++;
            _headerEnd++;
            RefreshGuide();
        }
    }

    function scrollLeft() {
        if (_headerStart > 0) {
            _headerStart--;
            _headerEnd--;
            RefreshGuide();
        }
    }

    function scrollUp() {
        if (_channelStart > 0) {
            _channelStart--;
            _channelEnd--;
            RefreshGuide();
        }
    }

    function scrollDown() {
        if (_channelEnd < _channels.length - 1) {
            _channelStart++;
            _channelEnd++;
            RefreshGuide();
        }
    }

	function Debug(msg)
	{
		var debug = document.getElementById("debug");
		if (debug)
			debug.innerHTML = msg;
	}

	function Debug2(msg)
	{
		var debug = document.getElementById("debug2");
		if (debug)
			debug.innerHTML = msg;
	}

	function RemoveGuide() {
        var container = document.getElementById("guideTable");
        var len = container.children.length;
        for (var i = 0; i < len; i++) {
            var tbl = container.children[i];
            if (i != 0) {
                var lastRow = tbl.rows.length - 1;
                for (var r = lastRow; r >= 0; r--) {
                    tbl.deleteRow(r);
                }
            }
        }
        for (var i = len-1; i >= 0; i--) {
            elem = container.children[i];
            elem.parentNode.removeChild(elem);
        }
	}

	function AddSpan(t1, span)
	{
		var t = t1 + span;
		return t;
	}
	
