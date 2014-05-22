package com.halkamalka.ever.eve.views;
import java.net.MalformedURLException;
import java.net.URL;

import org.eclipse.mylyn.commons.ui.dialogs.AbstractNotificationPopup;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.ever.eve.core.data.Product;
 
@SuppressWarnings("restriction")
public class NotificationPopUpUI extends AbstractNotificationPopup
{
 
  @SuppressWarnings("restriction")
public NotificationPopUpUI(Display display)
  {
    super(display);
  }
 
  @Override
  protected void createContentArea(Composite composite)
  {
    composite.setLayout(new GridLayout(1, true));
    Link linkEverNews = new Link(composite, 0);
    String googlenewsLink = "<a href=\"http://evernever.co.kr/ \">Ever & Ever  News</a>";
    linkEverNews.setText(googlenewsLink);
    linkEverNews.setSize(400, 100);
    composite.setBackgroundImage(getImage()); // TODO
 
    linkEverNews.addSelectionListener(new SelectionAdapter()
    {
      public void widgetSelected(SelectionEvent e) {
        try {
          PlatformUI.getWorkbench().getBrowserSupport()
            .getExternalBrowser().openURL(new URL(e.text));
        } catch (PartInitException e1) {
          e1.printStackTrace();
        } catch (MalformedURLException e1) {
          e1.printStackTrace();
        }
      }
    });
  }
 
   
  @Override
  protected String getPopupShellTitle()
  {
    return "Ever and Ever\nDatabase has been updated!";
  }
  
  @Override
  protected Image getPopupShellImage(int maximumHeight)
  {
	  return getImage();
  }
  
  private Image getImage() {
	  Image image = null;
	  Product product = DataManager.getInstance().getLatestProduct();
	  if(product != null) {
			
		  ImageLoader imageLoader = new ImageLoader();
		  ImageData[] imageData = imageLoader.load(product.getImage());
			
		  if(imageData.length > 0){
//			  imageData[0].scaledTo(imageData[0].width * maximumHeight / imageData[0].height, maximumHeight);
			  image = new Image(PlatformUI.getWorkbench().getDisplay(), imageData[0]);
		  }
	  }
	  return image;
  }
}