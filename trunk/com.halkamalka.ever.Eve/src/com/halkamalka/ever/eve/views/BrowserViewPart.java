package com.halkamalka.ever.eve.views;

import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;

public class BrowserViewPart extends ViewPart {

	public static final String ID = "com.halkamalka.ever.eve.views.BrowserViewPart";
//	private static final String OS = System.getProperty("os.name").toLowerCase();
	
	private Browser browser = null;
	
	public BrowserViewPart() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public void createPartControl(Composite parent) {
		// TODO Auto-generated method stub
		if(org.apache.commons.exec.OS.isFamilyWindows()) {
			browser = new Browser(parent, SWT.NONE);
		}
		else {
			browser = new Browser(parent, SWT.WEBKIT);
		}
		if(!browser.getJavascriptEnabled()) 
			browser.setJavascriptEnabled(true);
		
//		browser.setUrl("http://www.google.com");
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub
		browser.setFocus();
	}

	public void setUrl(String url) {
		browser.setUrl(url);
	}
	
	public void setTitleLabel(String title) {
		setPartName(title);
	}
	
//	public static boolean isWindows() {
//		return (OS.indexOf("win") >= 0);
//	}
}
