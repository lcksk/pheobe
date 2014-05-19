package com.halkamalka.ever.eve.core.data;

import java.util.Iterator;
import java.util.logging.Logger;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

public class Product {

	final private static Logger log = Logger.getLogger(Product.class.getName());
	
	private String name = null;
	private String description = null;
	private String image = null;
	private String price0 = null;
	private int hash = 0;

	public Product(int hash, String name, String description, String image, String price0) {
		this.image = image;
		this.hash = hash;
		this.name = name;
		this.price0 = price0;
		
		if(org.apache.commons.exec.OS.isFamilyWindows()) {
			Document doc = Jsoup.parse(description);	
			Elements elements = doc.select("img");
			for(Iterator<Element> it = elements.iterator(); it.hasNext(); ) {
				Element e = it.next();
				String tmp = null;
				//
				tmp = e.attr("data-cke-saved-src");
				tmp = tmp.replace("/", "\\");
				e.attr("data-cke-saved-src", tmp);

				//
				tmp = e.attr("src");
				tmp = tmp.replace("/", "\\");
				e.attr("src", tmp);
				
				description = doc.html();
			}

//			try {
//				description = new String(description.getBytes("UTF-8"), Charset.forName("EUC-KR"));
//			} catch (UnsupportedEncodingException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
		}
		
		this.description = description;
	}

	public String getDescription() {
		return description;
	}
	
	public String getImage() {
		return image;
	}
	
	public String getName() {
		return name;
	}
	
	public int getHash() {
		return hash;
	}
	
	public String getPrice0() {
		return price0;
	}
}
