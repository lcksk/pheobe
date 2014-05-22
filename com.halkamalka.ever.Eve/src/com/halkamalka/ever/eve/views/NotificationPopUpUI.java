package com.halkamalka.ever.eve.views;
import java.net.MalformedURLException;
import java.net.URL;

import org.eclipse.mylyn.commons.ui.dialogs.AbstractNotificationPopup;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
 
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
    String googlenewsLink = "This is a link to <a href=\"http://evernever.co.kr/ \">Ever & Ever  News</a>";
    linkEverNews.setText(googlenewsLink);
    linkEverNews.setSize(400, 100);
 
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
    return null;
  }
}