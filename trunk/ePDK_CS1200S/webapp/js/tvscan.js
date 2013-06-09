//interface/remote controls for tv scan screen

	var _curWidth;
	var _width = 700;
	var _height = 34;
	var _x = 50;
	var _y = 20;
	var _radius;
	var _contextScanBar = null;
	var _continueScan = false;
	var _scanBarStarted = false;

	function InitScanList(myObject)
	{
		var scanList = myObject.result;
		var busy = scanList.busy;
		var progress = myObject.result.progress;
		var totalServices = myObject.result.services;
		var tvServices = myObject.result.TVServices;
		var radioServices = myObject.result.radioServices;
		var dataServices = myObject.result.dataServices;
		var tranponders = myObject.result.transponders;
		
		if (!_scanBarStarted)
			return;
		
		_curWidth = Math.floor( (progress/100) * _width);
		UpdateScanBar();
		
		if (tranponders > 0)
			document.getElementById("tpChannel").innerHTML = "<span style='color:green;font-weight:normal;'>" +
				+ tranponders + "</span>";
		else
			document.getElementById("tpChannel").innerHTML = "<span style='color:white;font-weight:normal;'>" +
				+ tranponders + "</span>";

		if (tvServices > 0)
			document.getElementById("tvChannel").innerHTML = "<span style='color:green;font-weight:normal;'>" +
				+ tvServices + "</span>";
		else
			document.getElementById("tvChannel").innerHTML = "<span style='color:white;font-weight:normal;'>" +
				+ tvServices + "</span>";

		if (radioServices > 0)
			document.getElementById("radioChannel").innerHTML = "<span style='color:green;font-weight:normal;'>" +
				+ radioServices + "</span>";
		else
			document.getElementById("radioChannel").innerHTML = "<span style='color:white;font-weight:normal;'>" +
				+ radioServices + "</span>";
				
	}
	
	function OnScanBarClick()
	{
		if (_scanBarStarted)
			StopScanBar();
		else
			StartScanBar();
	}
	
	function StartScanBar()
	{
		var el = document.getElementById('scanBarInfo');
		if (el != null)
			el.innerHTML = 'Press Stop to stop channel searching'; 
		el = document.getElementById('scanBarStart')
		if (el != null)
			el.innerHTML = 'Stop<span></span>'; 
		el = document.getElementById('scanBarProgress')
		if (el != null)
			el.innerHTML = 'Searching in progress...'; 
			
		tvScanRpc();

		// start off tvScanRpcStage2 
		_initComplete = true;
		tvScanRpcStage2();
		
		_scanBarStarted = true;
	}
	
	function StopScanBar()
	{
		_scanBarStarted = false;
		_continueScan = false;
		_curWidth = 0;
		UpdateScanBar();
		var el = document.getElementById('scanBarInfo');
		if (el != null)
			el.innerHTML = 'Press Start to start channel searching'; 
		el = document.getElementById('scanBarStart')
		if (el != null)
			el.innerHTML = 'Start<span></span>'; 
		el = document.getElementById('scanBarProgress')
		if (el != null)
			el.innerHTML = '&nbsp;'; 
	}
	
	function HideScanBar()
	{
		if (_scanBarStarted)
			StopScanBar();
			
		_scanBarActive = false;

		var el = document.getElementById('scanBarStatus');
		if (el != null)
			el.className = "hidden";

		el = document.getElementById('scanBarInfo');
		if (el != null)
			el.className = "hidden";
			
		el = document.getElementById('footer');
		if (el != null)
			el.className = "hidden";

		el = document.getElementById('scanBarProgress');
		if (el != null)
			el.className = "hidden";

		el = document.getElementById('legend');
		if (el != null)
			el.className = "legendHidden";
	}

	function ShowScanBar()
	{
		var el = document.getElementById('scanBarStatus');
		if (el != null)
			el.className = "scanBarStatus";
			
		el = document.getElementById('scanBarInfo');
		if (el != null)
			el.className = "scanBarInfo";
			
		el = document.getElementById('footer');
		if (el != null)
			el.className = "footer";
			
		el = document.getElementById('scanBarProgress');
		if (el != null)
			el.className = "scanBarProgress";
			
		el = document.getElementById('legend');
		if (el != null)
		{
			el.className = "";
			el = document.getElementById('scanBarImg');
			if ((el != null) && (!_isTV))
				el.className = "legendHidden";
			el = document.getElementById('scanBarImgText');
			if ((el != null) && (!_isTV))
				el.className = "legendHidden";
		}
			
		el = document.getElementById('scanBar');
		if (!el || !el.getContext) {
			return;
		}

		_contextScanBar = el.getContext('2d');
		if (!_contextScanBar) {
			return;
		}

		_contextScanBar.font = "16px Verdana";
		var g = _contextScanBar.createLinearGradient(0,_y+_height,0,0);
		
		//from(rgba(16,96,192,0.75)), to(rgba(96,192,255,0.9)));
		
		g.addColorStop(0, '#4DA4F3');
		g.addColorStop(0.4, '#ADD9FF');
		g.addColorStop(1, '#79B');	//'#9ED1FF');
		_contextScanBar.fillStyle = g;
		_radius = _height/2;
		_curWidth = 0;
		UpdateScanBar();
		
		_scanBarActive = true;
	}

	function UpdateScanBar() {
		//_curWidth+=1;
		_contextScanBar.clearRect(_x-5,_y-5,_width+15,_height+15);
		ScanBarLayer(_contextScanBar, _x, _y, _width, _height, _radius);
		ScanBarRect(_contextScanBar, _x, _y, _curWidth, _height, _radius, _width);
		ScanBarText(_contextScanBar, _x, _y, _curWidth, _height, _radius, _width );

		if (_curWidth>=_width) {
			var el = document.getElementById('scanBarInfo');
			if (el != null)
				el.innerHTML = 'Press Start to start channel searching'; 
			el = document.getElementById('scanBarStart')
			if (el != null)
				el.innerHTML = 'Start<span></span>'; 
			el = document.getElementById('scanBarProgress')
			if (el != null)
				el.innerHTML = 'Searching completed'; 
			_scanBarStarted = false;
			_continueScan = false;
		}
	}

	function roundRect(ctx, x, y, width, height, radius) {
		ctx.beginPath();
		ctx.moveTo(x + radius, y);
		ctx.lineTo(x + width - radius, y);
		ctx.arc(x+width-radius, y+radius, radius, -Math.PI/2, Math.PI/2, false);
		ctx.lineTo(x + radius, y + height);
		ctx.arc(x+radius, y+radius, radius, Math.PI/2, 3*Math.PI/2, false);
		ctx.closePath();
		ctx.fill();
	}

	function ScanBarParentRect(ctx, x, y, width, height, radius) {
		ctx.beginPath();
		ctx.moveTo(1000, 1000);
		ctx.lineTo(1000, -1000);
		ctx.lineTo(-1000, -1000);
		ctx.lineTo(-1000, 1000);
		ctx.lineTo(1000, 1000);
		ctx.moveTo(x + radius, y);
		ctx.lineTo(x + width - radius, y);
		ctx.arc(x+width-radius, y+radius, radius, -Math.PI/2, Math.PI/2, false);
		ctx.lineTo(x + radius, y + height);
		ctx.arc(x+radius, y+radius, radius, Math.PI/2, 3*Math.PI/2, false);
		ctx.closePath();
		ctx.fill();
	}

	function ScanBarLayer(ctx, x, y, width, height, radius) {
		ctx.save();
		
		// shadow depths
		ctx.shadowOffsetX = 2;
		ctx.shadowOffsetY = 5;
		ctx.shadowBlur = 5;
		ctx.shadowColor = 'green';	//'#666'; - grey

		// add layer
		ctx.fillStyle = 'rgba(189,189,189,1)'; //'rgba(189,189,189,1)'; 
		roundRect(ctx, x, y, width, height, radius);

		// overlay layer with gradient
		ctx.shadowColor =  'rgba(0,0,0,0.1)';
		var lingrad = ctx.createLinearGradient(0,y+height,0,0);
		lingrad.addColorStop(0, 'rgba(255,255,255, 0.1)');
		lingrad.addColorStop(0.4, 'rgba(255,255,255, 0.7)');
		lingrad.addColorStop(1, 'rgba(255,255,255,0.4)');
		ctx.fillStyle = lingrad;
		roundRect(ctx, x, y, width, height, radius);

		ctx.fillStyle = '#F88';
		//ScanBarParentRect(ctx, x, y, width, height, radius);

		ctx.restore();
	}

	function ScanBarRect(ctx, x, y, width, height, radius, max) {
		var offset = 0;
		ctx.beginPath();
		if (width<radius) {
			offset = radius - Math.sqrt(Math.pow(radius,2)-Math.pow((radius-width),2));
			ctx.moveTo(x + width, y+offset);
			ctx.lineTo(x + width, y+height-offset);
			ctx.arc(x + radius, y + radius, radius, Math.PI - Math.acos((radius - width) / radius), Math.PI + Math.acos((radius - width) / radius), false);
		}
		else if (width+radius>max) {
			offset = radius - Math.sqrt(Math.pow(radius,2)-Math.pow((radius - (max-width)),2));
			ctx.moveTo(x + radius, y);
			ctx.lineTo(x + width, y);
			ctx.arc(x+max-radius, y + radius, radius, -Math.PI/2, -Math.acos((radius - (max-width)) / radius), false);
			ctx.lineTo(x + width, y+height-offset);
			ctx.arc(x+max-radius, y + radius, radius, Math.acos((radius - (max-width)) / radius), Math.PI/2, false);
			ctx.lineTo(x + radius, y + height);
			ctx.arc(x+radius, y+radius, radius, Math.PI/2, 3*Math.PI/2, false);
		}
		else {
			ctx.moveTo(x + radius, y);
			ctx.lineTo(x + width, y);
			ctx.lineTo(x + width, y + height);
			ctx.lineTo(x + radius, y + height);
			ctx.arc(x+radius, y+radius, radius, Math.PI/2, 3*Math.PI/2, false);
		}
		ctx.closePath();
		ctx.fill();

		// right border shadow
		if (width<max-1) {
			ctx.save();
			ctx.shadowOffsetX = 1;
			ctx.shadowBlur = 1;
			ctx.shadowColor = '#666';
			if (width+radius>max)
			  offset = offset+1;
			ctx.fillRect(x+width,y+offset,1,_height-offset*2);
			ctx.restore();
		}
	}

	function ScanBarText(ctx, x, y, width, height, radius, max) {
		ctx.save();
		ctx.fillStyle = 'rgba(255, 255, 255, 0.1)'; //'white';
		var text = Math.floor(width/max*100)+"%";
		var text_width = ctx.measureText(text).width;
		var text_x = x+width-text_width-radius/2;
		if (width<=radius+text_width) {
			text_x = x+radius/2;
		}
		ctx.fillText(text, text_x, y+22);
		ctx.restore();
	}
	