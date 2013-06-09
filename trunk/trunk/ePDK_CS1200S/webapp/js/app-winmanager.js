var menuActive = true;
var remoteInterval;
var keysLocked = false;

function clearRemoteInterval() {
	setTimeout(function () { keysLocked = false; }, 100);
}

var WindowManager = Class.extend({
    menu: null,
    init: function () {
        this.attachEvents();
        this.menu = new ApplicationMenu();
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
			//alert(evt.keyCode);
            switch (evt.keyCode) {
                case VK_OK:
                    if (menuActive)
                        self.menu.select();
                    break;
                case VK_RIGHT:
                    if (menuActive)
                        self.menu.select();
                    break;
                case VK_LEFT:
                case VK_LAST:
                    if (menuActive)
                        self.menu.goBack();
                    break;
                case VK_UP:
                    if (menuActive)
                        self.menu.moveUp();
                    break;
                case VK_DOWN:
                    if (menuActive)
                        self.menu.moveDown();
                    break;
                case VK_CHANNEL_UP:
					pressed = true;
                    if (menuActive)
                        self.menu.pageUp();
                    break;
                case VK_CHANNEL_DOWN:
                    if (menuActive)
                        self.menu.pageDown();
                    break;
                case VK_EXIT:
                    //$View.deactivate();
                    break;
                case VK_INFO:
                    if (hasInfo())
                        toggleInfo();
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

window.onunload = function() {
};

function hideAll() {
    document.getElementById("menu").className = "menu hidden";
    menuActive = false;
}

function showAll() {
    document.getElementById("menu").className = "menu";
    menuActive = true;
}

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
