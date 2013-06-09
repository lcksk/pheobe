//interface/remote controls for tvclient screen

var channelManagerActive = false;
var _channelGuideActive = false;
var _nowNextActive = false;
var _channelMenuActive = false;
var _scanBarActive = false;
var _subtitleActive = false;
var _audioActive = false;

var tvclient = Class.extend({
    menu: null,
    manager: null,
    init: function () {
        this.attachEvents();
        this.menu = new tvclientMenu();
		this.manager = new CmMenu();
        var self = this;
    },
    attachEvents: function () {
        var self = this;
        var keyDown = function (evt) {
			//if (_debug)
        	//Debug(evt.keyCode);
            switch (evt.keyCode) {
                case VK_RIGHT:
					if (_channelGuideActive)
						break;
					if (!_channelMenuActive)
						break;
					if (channelManagerActive) {
						self.manager.moveRight();
						break;
					}
				case 13:	// keyboard ENTER
                case VK_OK:
                case VK_PLAY:
					if (_channelGuideActive)
					{
						if (_tvAvail)
						{
							StartTV();
							break;
						}
						hideDetail();
						hideGuide();
						showMenu();
						self.menu.goBack();
						break;
					}
					
                    if (channelManagerActive) {
                        self.manager.select();
                        break;
                    }
					if (_scanBarActive)
					{
						OnScanBarClick();
						break;
					}
					if (_channelMenuActive)
						self.menu.select();
						
                    break;
				
				case 36: 		// keyboard home, treat as GUIDE
                case VK_EXIT:
					if (_scanBarActive)
					{
						HideScanBar();
						_scanBarActive = false;
					}
					if (_channelGuideActive)
					{
						if (_guideLoading)
							break;
						hideDetail();
						hideGuide();
						//showMenu();
						//self.menu.goBack();
					}
					else
					{
						self.menu.onItemSelected(1);
					}
					break;
                case VK_LEFT:
					if (_channelGuideActive)
						break;
					if (!_channelMenuActive)
						break;
					if (_nowNextActive) {
						hideNowNext();
						break;
					}
                case VK_LAST:	// currently not mapped
                    if (channelManagerActive) {
                        channelManagerActive = false;
						self.manager.hide();
                        break;
                    }
					else if (_channelGuideActive) {
						hideGuide();
                        break;
					}
                    self.menu.goBack();
                    break;
                case VK_UP:
				case 38:	// keyboard UP
					if (_subtitleActive) {
						MoveUpSubTitle();
						break;
					}
					if (_audioActive) {
						MoveUpAudio();
						break;
					}
					if (_channelGuideActive)
						break;
					if (!_channelMenuActive)
						break;
                    if (channelManagerActive) {
						if (_nowNextActive) {
							hideNowNext();
						}
                        self.manager.moveUp();
                        break;
                    }
					self.menu.moveUp();
					break;
                case VK_DOWN:
				case 40:	// keyboard DOWN
					if (_subtitleActive) {
						MoveDownSubTitle();
						break;
					}
					if (_audioActive) {
						MoveDownAudio();
						break;
					}
					if (_channelGuideActive)
						break;
					if (!_channelMenuActive)
						break;
                    if (channelManagerActive) {
						if (_nowNextActive) {
							hideNowNext();
						}
                        self.manager.moveDown();
                        break;
                    }
					self.menu.moveDown();
					break;
				case 33:	// keyboard PAGEUP
                case VK_CHANNEL_UP:
					if (_channelGuideActive)
						break;
					if (_subtitleActive)
						HideSubTitle();
                    if (channelManagerActive) {
						if (_nowNextActive)
							hideNowNext();
						self.manager.moveUp();
                        self.manager.select();
                    }
					else
					{
						//if (_channelSelection.length == 0)
						//	tvRpcSilent();
						if (_channelSelection.length > 0)
						{
							_selectedChannelIndex--;
							if (_selectedChannelIndex < 0)
								_selectedChannelIndex = _channelSelection.length-1;
							_channelSelection.onItemSelected(_selectedChannelIndex);
						}
					}
					break;
				case 34:	// keyboard PAGEDOWN
                case VK_CHANNEL_DOWN:
					if (_channelGuideActive)
						break;
					if (_subtitleActive)
						HideSubTitle();
                    if (channelManagerActive) {
						if (_nowNextActive)
							hideNowNext();
						self.manager.moveDown();
                        self.manager.select();
                    }
					else
					{
						//if (_channelSelection.length == 0)
						//	tvRpcSilent();
						if (_channelSelection.length > 0)
						{
							_selectedChannelIndex++;
							if (_selectedChannelIndex >= _channelSelection.length)
								_selectedChannelIndex = 0;
							_channelSelection.onItemSelected(_selectedChannelIndex);
						}
					}
                    break;
                case VK_GUIDE:
					if (_subtitleActive)
						HideSubTitle();
					if (_scanBarActive)
					{
						HideScanBar();
						_scanBarActive = false;
					}
					if (_guideLoading)
						break;
					if (_channelGuideActive){
						hideDetail();
						hideGuide();
						posMedia();
						Debug("");
						Debug2("");
						//showMenu();
						//self.menu.goBack();
					}
					else if (_channelMenuActive)
					{
						if (_nowNextActive)
							hideNowNext();
						if (channelManagerActive) {
							channelManagerActive = false;
							self.manager.hide();
						}							
						hideMenu();
						break;
					}
					else {
						if (_nowNextActive) {
							hideNowNext();
						}
						self.menu.onItemSelected(1);
					}
                    break;
				case 18:			// VK_MENU
					/* disable, do not handle VK_MENU:
					if (_guideLoading)
						break;
					if (_channelGuideActive){
						hideDetail();
						hideGuide();
						posMedia();
					}
					if (channelManagerActive) {
						channelManagerActive = false;
						self.manager.hide();
					}
					if (_channelMenuActive)
					{
						if (_nowNextActive)
							hideNowNext();
						hideMenu();
						break;
					}
					showMenu();
					self.menu.goBack();*/
					break;
				case 35:		// keyboard END key, treat as INFO
                case VK_INFO:
					if (_subtitleActive)
						HideSubTitle();
                    if (channelManagerActive) {
						if (_nowNextActive) {
							hideNowNext();
							break;
						}
						self.manager.moveRight();
						break;
					}
					if (!_channelMenuActive)
						ShowTVInfo()
                    break;
				case 403:
					//ChangeTableBkColor('A');
                    break;
				case 404:
				case VK_AUDIO:
					//ChangeTableBkColor('B');
					if (_scanBarActive)
					{
						HideScanBar();
						_scanBarActive = false;
					}
					if (_subtitleActive)
						HideSubTitle();
					if (_guideLoading)
						break;
					if (_channelGuideActive){
						hideDetail();
						hideGuide();
						posMedia();
						Debug("");
						Debug2("");
						//showMenu();
						//self.menu.goBack();
					}
					if (_audioActive)
						HideAudio();
					else
						GetAudioAttributes();
                    break;
				case 405:
                case 460:	// VK_SUBTITLE:
					//ChangeTableBkColor('C');
					if (_scanBarActive)
					{
						HideScanBar();
						_scanBarActive = false;
					}
					if (_audioActive)
						HideAudio();
					if (_guideLoading)
						break;
					if (_channelGuideActive){
						hideDetail();
						hideGuide();
						posMedia();
						Debug("");
						Debug2("");
						//showMenu();
						//self.menu.goBack();
					}
					if (_subtitleActive)
						HideSubTitle();
					else
						GetSubTitle();
                    break;
				case 406:	// TV Scan
					//ChangeTableBkColor('D');
					if (_guideLoading)
						break;
					if (_subtitleActive)
						HideSubTitle();
					if (_channelGuideActive)
					{
						hideDetail();
						hideGuide();
					}
					Debug("");
					Debug2("");
					channelBoxHide();
					if (_scanBarActive)
					{
						HideScanBar();
					}
					else
					{
						ShowScanBar();
					}
                    break;

                case VK_PAUSE:
                    break;
                case VK_MUTE:
                	MuteAudio();
                    break;
                case VK_VOLUME_DOWN:
                    break;
                case VK_VOLUME_UP:
                    break;
                case VK_0:
                case VK_1:
                case VK_2:
                case VK_3:
                case VK_4:
                case VK_5:
                case VK_6:
                case VK_7:
                case VK_8:
                case VK_9:
                case 0:
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

function onGuideClick()
{
	if (_scanBarActive)
	{
		HideScanBar();
		_scanBarActive = false;
	}
	if (_guideLoading)
		return;
	if (_channelGuideActive){
		hideDetail();
		hideGuide();
		posMedia();
		Debug("");
		Debug2("");
	}
	
	_tvclient.menu.onItemSelected(1);
}



