package com.halkamalka.ever.eve.views;
import java.io.File;

import org.eclipse.mylyn.commons.ui.dialogs.AbstractNotificationPopup;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.ever.eve.core.data.Product;
import com.halkamalka.ever.eve.preferences.PreferenceConstants;
 
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
    Label label = new Label(composite, SWT.NONE);
    label.setBounds(0, 0, 400, 400);
//    label.setImage(getImage(400));

    Link linkEverNews = new Link(composite, 0);
    String evernewsLink = "<a href=\"http://evernever.co.kr/ \">Ever and Ever  News</a>";
    linkEverNews.setText(evernewsLink);
    linkEverNews.setSize(400, 100);
    linkEverNews.addSelectionListener(new SelectionAdapter()
    {
      public void widgetSelected(SelectionEvent e) {
        try {
//          PlatformUI.getWorkbench().getBrowserSupport()
//            .getExternalBrowser().openURL(new URL(e.text));
        	IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
			page.showView(BrowserViewPart.ID);
			IViewPart part = page.findView(BrowserViewPart.ID);
			if(!(part instanceof BrowserViewPart)) {
				return;
			}
			BrowserViewPart browser = (BrowserViewPart) part;
			browser.setUrl(e.text);
			
        }
        catch (PartInitException e1) {
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
	  return getImage(maximumHeight);
  }
  
  private Image getImage(int maximumHeight) {
	  Image image = null;
	  Product product = DataManager.getInstance().getLatestProduct();
	  if(product != null) {
			
		  ImageLoader imageLoader = new ImageLoader();
		  System.out.println("image - " + product.getImage());
		  ImageData[] imageData = imageLoader.load(PreferenceConstants.P_EVE_HOME + File.separator + product.getImage());
			
		  if(imageData.length > 0){
//			  imageData[0].scaledTo(imageData[0].width * maximumHeight / imageData[0].height, maximumHeight);
			  image = new Image(PlatformUI.getWorkbench().getDisplay(), imageData[0]);
		  }
	  }
	  return image;
  }
}