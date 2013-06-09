
var _default2dImages = ["../images/server_folder160x240.png", "../images/media_folder160x240.png", "../images/videos256.png"];
var _default3dImages = ["./images/server_folder160x240.png", "./images/media_folder160x240.png", "./images/videos256.png"];

var Coverflow = Class.extend({
    init: function() {
        this._super();
    },
    show: function() {
    },
    hide: function() {
    },
    clearImages: function() {
    },
    loadImages: function(container, startIndex, count) {
    },
    beginMoveTo: function(index) {
    },
    moveTo: function(index) {
    },
	setClass: function(name) {
		var coverflowElement = document.getElementById('coverflow');
		coverflowElement.className = name;
	}
});

var Coverflow2d = Coverflow.extend({
    _image: null,
    _imageList: null,
    imageListCount : null,
    init: function() {
        var coverflowElement = document.getElementById('coverflow');
        coverflowElement.className = 'coverflow2d';
        this._image = document.createElement('img');
        this._image.className = 'image';
        this._image.style.width = '100%';
        this._image.style.height = '100%';
        //this._image.style.visibility = 'hidden';
        coverflowElement.appendChild(this._image);
        var cachedImage = new Image();
        for (var i = 0; i < _default2dImages.length; i++) {// preload default images
            cachedImage.src = _default2dImages[i];
        }
        this._imageList = [];
    },
    clearImages: function() {
        this._imageList.length = 0;
        this._image.style.visibility = 'hidden';
    },
    loadImages: function(container, startIndex, loadedCount, totalCount, on) {
        if (totalCount > 0) {
            this._image.style.visibility = 'visible';
        }
        imageListCount = totalCount;
        for (var i = startIndex; i < startIndex + loadedCount; i++) {
			if(on)
				this._imageList[i] = container.children[i].poster;
			else
				this._imageList[i] = "./images/folder_grey.png";
        }
    },
    moveTo: function(index) {
	try {
        if (index > imageListCount)
            this._image.src = this._imageList[0];
        else
            this._image.src = this._imageList[index];
		}
			catch(e) {
			
			
			}
    },
	show: function() {
		var coverflowElement = document.getElementById('coverflow');
		coverflowElement.className = "coverflow2d";
	},
	hide: function() {
		var coverflowElement = document.getElementById('coverflow');
		coverflowElement.className = "hidden";
	}
});

var Coverflow3d = Coverflow.extend({
    _plugin: null,
    _images: null,
    init: function() {
        var coverflowElement = document.getElementById('coverflow');
        coverflowElement.className = 'coverflow3d';
        this._plugin = document.createElement('embed');
        this._plugin.className = 'plugin';
        this._plugin.width = 640;
        this._plugin.height = 480;

        //Disable plugin temporarily by commenting out the plugin.type
        this._plugin.type = 'application/x-dfb3dcoverflow';
        coverflowElement.appendChild(this._plugin);
        //this._plugin.defaultImage = "./images/wait.jpg";
        this._images = [];
    },
    destroy: function() {
        var coverflowElement = document.getElementById('coverflow');
        coverflowElement.removeChild(this._plugin);
        delete this;
    },
    show: function() {
        this._plugin.fadeSpeed = 3;
        this._plugin.fadeIn();
    },
    hide: function() {
        this._plugin.fadeSpeed = 10;
        this._plugin.fadeOut();
    },
    test: function() {
        try {
            this._plugin.buffer(_default3dImages);
            return true;
        }
        catch (e) {
            return false;
        }
    },
    clearImages: function() {
        this._images.length = 0;
    },
    loadImages: function(container, startIndex, loadedCount, totalCount) {
        var image;
		var last = startIndex + loadedCount ;
        
		alert("total count: " + totalCount);
		alert("start index: " + startIndex);
		if(startIndex == 0)
        {
            if(totalCount != 0)
    			this._plugin.createNewFolder(totalCount);
            else
            {
    			this._plugin.createNewFolder(1);
                this._plugin.changeImage(0, _default3dImages[1]);
            }
            
        }
		
      for (var i = startIndex; i < startIndex + loadedCount; i++) {
        image = container.children[i].poster;
        if (image.indexOf("../") === 0)
          this._images[i] = image.slice(1); // HACK to bypass the ../
        else
          this._images[i] = image;
        alert(i+ "  " + this._images[i]);
        this._plugin.changeImage(i, this._images[i]);  
      }
	
    },
    beginMoveTo: function(index) {
        this._plugin.setFocusedIndex(index);
    },
	show: function() {
	},
	hide: function() {
	}});

function createCoverflowControl() {
    var coverflow = new Coverflow3d();
    if (coverflow.test() == false) {
        coverflow.destroy();
        coverflow = new Coverflow2d();
    }
    return coverflow;
}
