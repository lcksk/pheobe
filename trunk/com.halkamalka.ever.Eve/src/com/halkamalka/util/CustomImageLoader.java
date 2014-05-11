package com.halkamalka.util;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;

import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.ui.PlatformUI;

public class CustomImageLoader {

	public static Image getImage(URL url) {
		Image image = null;
		
		try {
			ImageLoader imageLoader = new ImageLoader();
			InputStream inputstream = url.openStream();
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
			//System.exit(0);
		}
		return image;
	}
}