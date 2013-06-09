// TOUCH-EVENTS SINGLE-FINGER SWIPE-SENSING JAVASCRIPT
// Courtesy of PADILICIOUS.COM and MACOSXAUTOMATION.COM
var triggerElementID = null; // this variable is used to identity the triggering element
var fingerCount = 0;
var startX = 0;
var startY = 0;
var curX = 0;
var curY = 0;
var deltaX = 0;
var deltaY = 0;
var horzDiff = 0;
var vertDiff = 0;
var minLength = 20; // the shortest distance the user may swipe
var swipeLength = 0;
var swipeAngle = null;
var swipeDirection = null;
var time = -10000;

function touchStart(event,passedName) {
	var now = new Date();
	//if(now.getTime() - time < 130) {
	//}
	//time = now.getTime();		
	//event.preventDefault();	// disable the standard ability to select the touched object
	fingerCount = event.touches.length;	// get the total number of fingers touching the screen
	// since we're looking for a swipe (single finger) and not a gesture (multiple fingers),
	// check that only one finger was used
	if ( fingerCount == 1 ) {
		// get the coordinates of the touch
		startX = event.touches[0].pageX;
		startY = event.touches[0].pageY;
		// store the triggering element ID
		triggerElementID = passedName;
	} else {
		// more than one finger touched so cancel
		touchCancel(event);
	}
}
	
function touchMove(event) {
	event.preventDefault();
	if ( event.touches.length == 1 ) {
		curX = event.touches[0].pageX;
		curY = event.touches[0].pageY;
	} else {
		touchCancel(event);
	}
}
		
function touchEnd(event) {
	// check to see if more than one finger was used and that there is an ending coordinate
	if ( fingerCount == 1 && curX != 0 ) {
		event.preventDefault();
		// use the Distance Formula to determine the length of the swipe
		swipeLength = Math.round(Math.sqrt(Math.pow(curX - startX,2) + Math.pow(curY - startY,2)));
		// if the user swiped more than the minimum length, perform the appropriate action
		if ( swipeLength >= minLength ) {
			caluculateAngle();
			determineSwipeDirection();
			processingRoutine();
			//Debug("touchEnd1, swipeLength=" + swipeLength);
			touchCancel(event); // reset the variables
		} else {
			//Debug("touchEnd2, swipeLength=" + swipeLength);
			touchCancel(event);
		}	
	} else {
		//Debug("touchEnd3, swipeLength=" + swipeLength);
		touchCancel(event);
	}
}
	
function touchCancel(event) {
	// reset the variables back to default values
	fingerCount = 0;
	startX = 0;
	startY = 0;
	curX = 0;
	curY = 0;
	deltaX = 0;
	deltaY = 0;
	horzDiff = 0;
	vertDiff = 0;
	swipeLength = 0;
	swipeAngle = null;
	swipeDirection = null;
	triggerElementID = null;
}

function caluculateAngle() {
	var X = startX-curX;
	var Y = curY-startY;
	var Z = Math.round(Math.sqrt(Math.pow(X,2)+Math.pow(Y,2))); //the distance - rounded - in pixels
	var r = Math.atan2(Y,X); //angle in radians (Cartesian system)
	swipeAngle = Math.round(r*180/Math.PI); //angle in degrees
	if ( swipeAngle < 0 ) { swipeAngle =  360 - Math.abs(swipeAngle); }
}
		
function determineSwipeDirection() {
	if ( (swipeAngle <= 45) && (swipeAngle >= 0) ) {
		swipeDirection = 'left';
	} else if ( (swipeAngle <= 360) && (swipeAngle >= 315) ) {
		swipeDirection = 'left';
	} else if ( (swipeAngle >= 135) && (swipeAngle <= 225) ) {
		swipeDirection = 'right';
	} else if ( (swipeAngle > 45) && (swipeAngle < 135) ) {
		swipeDirection = 'down';
	} else {
		swipeDirection = 'up';
	}
}

function processingRoutine() {
	var swipedElement = document.getElementById(triggerElementID);

	if ( swipeDirection == 'left' ) {
		moveLeft();
	} else if ( swipeDirection == 'right' ) {
		moveRight();
	} else if ( swipeDirection == 'up' ) {
		moveUp();
	} else if ( swipeDirection == 'down' ) {
		moveDown();
	} else {
	}
}
	
