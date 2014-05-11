package com.halkamalka.ever.eve.core.data;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Iterator;
import java.util.logging.Logger;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

public class Product {

	final private static Logger log = Logger.getLogger(Product.class.getName());
	
	private String major = null;
	private String minor = null;
	private String description = null;
	private String image = null;
	
	public Product() {
		// TODO Auto-generated constructor stub
	}

	public Product(String major, String minor, String description, String image) {
		this.major = major;
		this.minor = minor;
		this.image = image;
		
		if(org.apache.commons.exec.OS.isFamilyWindows())
		{
			Document doc = Jsoup.parse(description);	
			Elements elements = doc.select("img");
			for(Iterator<Element> it = elements.iterator(); it.hasNext(); ) {
				Element e = it.next();
				String tmp = null;
				tmp = e.attr("data-cke-saved-src");
				tmp = new String(tmp.getBytes(StandardCharsets.UTF_8), Charset.forName("euc-kr"));
				e.attr("data-cke-saved-src", tmp);
				log.info("****" + tmp);
				tmp = e.attr("src");
				tmp = new String(tmp.getBytes(StandardCharsets.UTF_8), Charset.forName("euc-kr"));
				e.attr("src", tmp);
				log.info("****" +tmp);
				description = doc.html();
				log.info("****" +description);
			}
		}
		
		this.description = description;
	}

	public String getMajor() {
		return major;
	}
	
	public String getMinor() {
		return minor;
	}
	
	public String getDescription() {
		return description;
	}
	
	public String getImage() {
		return image;
	}
}
