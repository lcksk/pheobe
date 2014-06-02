package com.halkamalka.util;


import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.ui.PlatformUI;

public class CustomSharedImage {

//	public enum SharedImage {
//		IMG_PACKAGE,
//		NUMOF_IMGS
//	}
	
//	public static final String[] paths = new String[1];
	
//	paths[0] = "";

//	public static final String IMG_WARN = "red_bug.gif";
	public static final String IMG_WARN = "bell.png";
	public static final String IMG_PASSED = "passed.png";
//	public static final String IMG_MSG_WARN = "message_warning.gif";
	public static final String IMG_MSG_WARN = "detail_contents.png";
	
	private Map<String, Image> map;
	
	private static CustomSharedImage manager = new CustomSharedImage();
	public static CustomSharedImage getInstance() {
		if(manager == null) {
			manager = new CustomSharedImage();
		}
		return manager;
	}


	private CustomSharedImage() {
		
		map = new HashMap<String, Image>();
		init();
	}
	
	private void init() {
//		load(IMG_PACKAGE);
//		load(IMG_PACKAGEFOLDER);
//		load(IMG_PHOTO_CONTENT);
//		load(IMG_VIDEO_CONTENT);
//		load(IMG_AUDIO_CONTENT);
//		load(IMG_STOP);
		load(IMG_WARN);
		load(IMG_PASSED);
		load(IMG_MSG_WARN);
	}

	private void load(String id) {
		Image img = _getImage(id);
		map.put(id, img);
	}

	public Image getImage(String id) {
		return map.get(id);
	}
	
	private Image _getImage(String path) {
		
		Image image = null;
		
		try {
			InputStream inputstream = getClass().getResourceAsStream("/icons/" + path);
			
			ImageLoader imageLoader = new ImageLoader();
			ImageData[] imageData = imageLoader.load(inputstream);
			if(imageData.length > 0)
			{
				image = new Image(PlatformUI.getWorkbench().getDisplay(), imageData[0]);		
				if(inputstream != null) {
					inputstream.close();
					inputstream = null;
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(0);
		}
		return image;
	}	
	
}

