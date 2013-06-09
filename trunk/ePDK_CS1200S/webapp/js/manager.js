//copy of menu.js, except manager instead of menu

var Manager = Class.extend({
    _manager: null,
    _highlight: null,
    _pageSize: 0,
    startIndex: 0,
    focusedIndex: 0,
    selectedIndex: -1,
    init: function (pageSize) {
        this._manager = document.getElementById('manager');
        this._manager.className = 'manager hidden';
        this._highlight = document.createElement('img');
        this._highlight.className = 'highlight';
        this._manager.appendChild(this._highlight);
        for (var i = 0; i < pageSize; i++) {
            var item = document.createElement('div');
            item.className = 'item';
            item.managerItem = null;
            this._manager.appendChild(item);
        }
        this._pageSize = pageSize;
    },
    show: function() {
        this._manager.className = 'manager';
    },
    hide: function(hideCompleteCallback) {
        this._manager.addEventListener('webkitTransitionEnd',
            function() {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
                if (hideCompleteCallback != null)
                    hideCompleteCallback();
            }, false);
        this._manager.className = 'manager hidden';
    },
    goHome: function() {
        this.startIndex = 0;
        this.selectedIndex = -1;
        this.refresh();
        this.setItemFocus(0);
    },
    moveUp: function() {
        var prevIndex = this.focusedIndex - 1;
        if (prevIndex >= 0) {
            if ((prevIndex - this.startIndex) < 0)
                this.scroll(-1);
            else
                this.setItemFocus(prevIndex);
        }
    },
    moveDown: function() {
        var nextIndex = this.focusedIndex + 1;
        if (nextIndex < this.getItemCount()) {
            if ((nextIndex - this.startIndex) >= this._pageSize)
                this.scroll(1);
            else
                this.setItemFocus(nextIndex);
        }
    },
    pageUp: function() {
        this.scroll(this._pageSize * -1);
    },
    pageDown: function() {
        this.scroll(this._pageSize);
    },
    scroll: function(offset) {
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
    setItemFocus: function(index) {
        var item = this.getItem(index);
        if (item == null)
            return;
        this.focusedIndex = index;
        this.onItemFocusChanging(index);
        if (item.element.offsetTop == this._highlight.offsetTop && 
            item.element.offsetHeight == this._highlight.offsetHeight) {
            this.onItemFocused(index);
        }
        else {
            var self = this;
            this._highlight.addEventListener('webkitTransitionEnd', function() {
                this.removeEventListener('webkitTransitionEnd', arguments.callee, false);
                self.onItemFocused(index);
            }, false);
            this._highlight.style.top = item.element.offsetTop;
            this._highlight.style.height = item.element.offsetHeight;
        }
    },
    select: function() {
        var managerItem;
        if (this.selectedIndex >= 0) {
            managerItem = this.getItem(this.selectedIndex);
            if (managerItem != null && managerItem.element != null)
                managerItem.element.className = 'item';
        }
        this.selectedIndex = this.focusedIndex;
        managerItem = this.getItem(this.selectedIndex);
        if (managerItem != null && managerItem.element != null)
            managerItem.element.className = 'item selected';
        this.onItemSelected(this.selectedIndex);
    },
    refresh: function() {
        var selectedItem = null;
        var offset = this.startIndex;
        var itemCount = this.getItemCount();
        if (itemCount == 0)
            return;
        if (this.selectedIndex >= 0)
            selectedItem = this.getItem(this.selectedIndex);
        var child = this._manager.children[0];
        while ((child = child.nextSibling)) {
            child.className = 'item';
            if (offset < itemCount) {
                var managerItem = this.getItem(offset);
                if (managerItem != null) {
                    child.innerHTML = managerItem.title;
                    child.managerItem = managerItem;
                    if (child.managerItem == selectedItem)
                        child.className = 'item selected';
                    child.style.visibility = 'visible';
                    managerItem.element = child;
                }
                else
                    child.style.visibility = 'hidden';
            }
            else {
                child.style.visibility = 'hidden';
            }
            offset++;
        }
    },
    onItemFocusChanging: function(index) {
    },
    onItemFocused: function(index) {
    },
    onItemSelected: function(index) {
    }
});

var ManagerItem = Class.extend({
    title: null,
    element: null,
	id: null,
	name: null,
	channelNumber: null,
	uri: null,
    init: function() { }
});
