var t = new Array();
var iCount = 0;
function resize() {
	iCount++;
	if(iCount == 10) {
	//this.width = "";
    //this.height = "";
	}
}

var Menu = Class.extend({
    _container: null,
    _header: null,
    _menu: null,
    _highlight: null,
    _pageSize: 0,
    startIndex: 0,
    focusedIndex: 0,
    selectedIndex: -1,
    init: function (pageSize) {
        this._container = document.getElementById('menu');
        this._container.className = 'menu';
        this._header = document.createElement('header');
        this._header.className = 'header';
        this._container.appendChild(this._header);
        this._menu = document.createElement('menuItems');
        this._container.appendChild(this._menu);
        this._highlight = document.createElement('highlight');
		 this._highlight.id = 'highlight';
        this._highlight.className = 'highlight';
        this._menu.appendChild(this._highlight);
        for (var i = 0; i < pageSize; i++) {
            var item = document.createElement('div');
            item.className = 'item';
            item.menuItem = null;
			item.setAttribute('index', i);
            item.onclick = function () {
				 var toSelect = _videoBrowser.menu.startIndex + +this.getAttribute('index');
				 if(toSelect == _videoBrowser.menu.focusedIndex)
					 _videoBrowser.menu.select();
				 else
					 _videoBrowser.menu.setItemFocus(toSelect);
           };
		   var br = document.createElement('br');
			var img = document.createElement('img');
			img.className = 'imgIcon';
			img.src = 'images/dlna256.png';
			img.onload = resize;
			var title = document.createElement('p');
			title.className = 'title';
			var preview = document.createElement('video');
			preview.className = 'preview';
			preview.loop = "loop";
			var indicator = document.createElement('p');
			indicator.className = 'hidden pipPosition';
			indicator.innerHTML = 'Mosaic not available for this selection';
			
			item.appendChild(br);
			item.appendChild(img);
			item.appendChild(title);
			item.appendChild(preview);
			item.appendChild(indicator);
            this._menu.appendChild(item);
        }
        this._pageSize = pageSize;
    },
    show: function () {
        this._container.className = 'menu';
    },
    hide: function (hideCompleteCallback) {
        this._container.addEventListener('webkitTransitionEnd',
            function () {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
                if (hideCompleteCallback != null)
                    hideCompleteCallback();
            }, false);
        this._container.className = 'menu hidden';
    },
    setHeaderText: function (txt) {
        this._header.width = '500px';
        this._header.innerHTML = txt;
    },
    goHome: function () {
        this.startIndex = 0;
        this.selectedIndex = -1;
        this.refresh();
        this.setItemFocus(0);
    },
    moveUp: function () {
        var prevIndex = this.focusedIndex - 1;
		if (prevIndex - 2 >= 0 && prevIndex + 3 < this.getItemCount()) {
			this.shift(-1);
			this.setItemFocus(prevIndex);
			
		} else	
        if (prevIndex >= 0) {
            if ((prevIndex - this.startIndex) < 0)
                this.scroll(-1);
            else
                this.setItemFocus(prevIndex);
        }
    },
    moveDown: function () {
        var nextIndex = this.focusedIndex + 1;
		if (nextIndex + 2 < this.getItemCount()) {
			if ((nextIndex - this.startIndex) >= 3)
                this.shift(1);
				this.setItemFocus(nextIndex);
		} else 
        if (nextIndex < this.getItemCount()) {
            if ((nextIndex - this.startIndex) >= this._pageSize)
                this.scroll(1);
            else
                this.setItemFocus(nextIndex);
        }
    },
    pageUp: function () {
        this.scroll(this._pageSize * -1);
    },
    pageDown: function () {
        this.scroll(this._pageSize);
    },
    scroll: function (offset) {
        var focusedOffset = this.focusedIndex - this.startIndex;
        var newIndex = this.startIndex + offset;
        if (newIndex < 0) {
            newIndex = 0;
            if (newIndex == this.startIndex)
                focusedOffset = 0;
        }
        else {
            var itemCount = this.getItemCount();
            var lastIndex = itemCount - this._pageSize;
            if (newIndex > lastIndex) {
                newIndex = lastIndex;
                if (newIndex == this.startIndex)
                    focusedOffset = this._pageSize - 1;
            }
        }
        this.startIndex = newIndex;
        this.refresh();
        this.setItemFocus(this.startIndex + focusedOffset);
    },
	shift: function(offset) {
	    //var shifted = false;
		if(pip || mosaic) {
			pip = false;
			mosaic = false;
			stopAllPreviews();
		}
		var focusedOffset = this.focusedIndex - this.startIndex;
        var newIndex = this.startIndex + offset;
        if (newIndex < 0) {
            newIndex = 0;
            if (newIndex == this.startIndex)
                focusedOffset = 0;
        }
        else {
            var itemCount = this.getItemCount();
            var lastIndex = itemCount - this._pageSize;
            if (newIndex > lastIndex) {
                newIndex = lastIndex;
                if (newIndex == this.startIndex)
                    focusedOffset = this._pageSize - 1;
            }
        }
        this.startIndex = newIndex;
        this.refresh();
	},
    setItemFocus: function (index) {
		if (index == -1) {
            this._highlight.style.height = 0;
            return;
        }
        var item = this.getItem(index);
        if (item == null)
            return;
        this.focusedIndex = index;
        this.onItemFocusChanging(index);
        if (item.element.offsetTop == this._highlight.offsetTop &&
            item.element.offsetHeight == this._highlight.offsetHeight && 
			item.element.offsetLeft == this._highlight.offsetLeft) {
            this.onItemFocused(index);
        }
        else {
            var self = this;
            this._highlight.addEventListener('webkitTransitionEnd', function () {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
                self.onItemFocused(index);
            }, false);
            this._highlight.style.top = item.element.offsetTop;
            this._highlight.style.height = item.element.offsetHeight;		
			this._highlight.style.left = item.element.offsetLeft;
        }
    },
    select: function () {
        var menuItem;
        if (this.selectedIndex >= 0) {
            menuItem = this.getItem(this.selectedIndex);
            if (menuItem != null && menuItem.element != null)
                menuItem.element.className = 'item';
        }
        this.selectedIndex = this.focusedIndex;
        menuItem = this.getItem(this.selectedIndex);
        if (menuItem != null && menuItem.element != null)
            menuItem.element.className = 'item selected';
        this.onItemSelected(this.selectedIndex);
    },
    refresh: function () {
        var selectedItem = null;
        var offset = this.startIndex;
        var itemCount = this.getItemCount();
        if (itemCount == 0) {
            this.setItemFocus(-1);
        }
        if (this.selectedIndex >= 0)
            selectedItem = this.getItem(this.selectedIndex);
        var child = this._menu.children[0];

        while ((child = child.nextSibling)) {
            child.className = 'item';
            if (offset < itemCount) {
                var menuItem = this.getItem(offset);
                if (menuItem != null) {
					child.children[1].src = menuItem.poster;
					if(menuItem.title.length < 20)
						child.children[2].innerHTML = "<br>" + menuItem.title;
					else
						child.children[2].innerHTML = "<br>" + menuItem.title.substring(0, 18) + " ...";
					child.menuItem = menuItem;
                    if (child.menuItem == selectedItem)
                        child.className = 'item selected';
                    child.style.visibility = 'visible';
                    menuItem.element = child;
                }
                else {
                    child.style.visibility = 'hidden';
                }
            }
            else {
                child.style.visibility = 'hidden';
            }
            offset++;
        }
    },
    onItemFocusChanging: function (index) { },
    onItemFocused: function (index) { },
    onItemSelected: function (index) { }
});

var MenuItem = Class.extend({
    title: null,
    element: null,
    init: function() { }
});

function click(value) {
    var k = " " + value.href;
    while (k.slice(0, 1) != "#") {
        k = k.slice(1);
    }
    k = k.slice(1);
    if (_videoBrowser.menu.focusedIndex == k) {
        _videoBrowser.menu.onItemSelected(k);
    }
    else
        _videoBrowser.menu.setItemFocus(k);
}

function getEvent(e) {
    var event = e || window.event;
    if (!event.target) {
        event.target = event.srcElement
    }
    return event;
}