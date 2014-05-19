package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.logging.Logger;

import javax.xml.parsers.ParserConfigurationException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import com.halkamalka.ever.eve.preferences.PreferenceConstants;

public class DomBuilder {
	final private static Logger log = Logger.getLogger(DomBuilder.class.getName());
	Document html;
	String name;

	public DomBuilder(String name) {
		this.name = name;
	}
	
	
	@Deprecated
	public void build(Data data, Product[] product) throws ParserConfigurationException {
		Document doc = Jsoup.parse(loadTemplate());
		Document sub = Jsoup.parse(loadSubTemplate());
//		Element body = html.createElement("body");

		if(org.apache.commons.exec.OS.isFamilyWindows()) {
			// replace encoding to euc-kr
			Element meta = doc.select("meta").first();
			meta.attr("content", "text/html; charset=euc-kr");
		}
		
		System.out.println(html.toString());

		for(int i = 0; i < product.length; i++) {
			
			Product p = product[i];
//			Element majorId = doc.select("#majorId").first();
//			majorId.text(p.getMajor());
			
//			Element minorId = doc.select("#minorId").first();
//			minorId.text(p.getMinor());
			
			Element scope = doc.select("#scope").first();
			
			Element value = doc.select("#value").first();
//			value.text("asdd");
			
			Element result = doc.select("#result").first();
			if(data.getPrescription() != null) {
				result.text(data.getPrescription());
			}
			
			Element productImage = sub.select("#productImage").first();
			productImage.attr("src", "data:image/*;base64,"+p.getImage());
			productImage.attr("src", p.getImage());
//			productImage.html("<p>lorem ipsum</p>");

			Element intro = sub.select("#productDescription").first();
			intro.html(replacePath(p.getDescription()));

			Element products = doc.select("#products").first();
			products.after(sub.html());
			
			System.out.println(doc.html());
		}
		
//		getBound(data);
		batchJQuery(DataManager.getInstance().getTempPath());
		save(DataManager.getInstance().getTempPath(), doc.html());
	}
	
	public void build(Data data, Bound[] bound) throws ParserConfigurationException {

		Document doc = Jsoup.parse(loadTemplate());
		Document sub = Jsoup.parse(loadSubTemplate());

		if(org.apache.commons.exec.OS.isFamilyWindows()) {
			// replace encoding to euc-kr
			Element meta = doc.select("meta").first();
			meta.attr("content", "text/html; charset=euc-kr");
		}

		Element result = doc.select("#result").first();
		if(data.getPrescription() != null) {
			result.text(data.getPrescription());
		}

		Element major = doc.select("#major").first();
		major.text(data.getName());
		
		
		Element minor = doc.select("#minor").first();
		
		Element status = doc.select("#degree").first();
		
		for(int i = 0; i < bound.length; i++) {
			Bound b = bound[i];
			
//			TODO
			minor.text(b.getMinor());
			
			switch(b.getStatus()) {
			case -3:
				status.attr("src", "YC01.gif");
			break;
			case -2:
				status.attr("src", "YC02.gif");
			break;
			case -1:
				status.attr("src", "YC03.gif");
			break;
			case 0:
				status.attr("src", "YC04.gif");
			break;
			case 1:
				status.attr("src", "YC05.gif");
			break;
			case 2:
				status.attr("src", "YC06.gif");
			break;
			case 3:
				status.attr("src", "YC07.gif");
			break;
			}
			

			log.info("@@@@@@@@@@@@@@@@@@@@ " + b.getMajor());
			
//			Product[] products_ = DataManager.getInstance().getProducts(b.getMajor(), null);
			Product[] products = DataManager.getInstance().getProducts(b.getProductName(), null);
			
			for(int j = 0; j < products.length; j++) {
				Product p = products[j];
				log.info("@@@@@@@@@@@  " + p.getName() + "num : " + products.length);
				Element intro = sub.select("#productDescription").first();
				intro.html(replacePath(p.getDescription()));

				
				
//				Element $products = doc.select("#products").first();
				Element $products = doc.select("#subContainer").first();
				
				Element productName = sub.select("#productName").first();
				productName.html(p.getName());
				
				Element productImage = sub.select("#productImage").first();
//				productImage.attr("src", "data:image/*;base64,"+ product.getImage());
				productImage.attr("src", p.getImage());
				
				Element productPrice = sub.select("#productPrice").first();
				productPrice.html(p.getPrice0());
				
				$products.append(sub.html());
			}
			
//			Element minorId = doc.select("#minorId").first();
//			minorId.text(p.getMinor());
			
//			Element scope = doc.select("#scope").first();
			
//			Element value = doc.select("#value").first();
//			value.text("asdd");
			

			

//			productImage.attr("src", p.getImage());
//			productImage.html("<p>lorem ipsum</p>");

			
			System.out.println(doc.html());
		}
		
//		getBound(data);
		batchJQuery(DataManager.getInstance().getTempPath()); // TODO
		save(DataManager.getInstance().getTempPath(), doc.html());
	}

	private String replacePath(String str) {
		if(org.apache.commons.exec.OS.isFamilyWindows()){
			return str.replace("\\userfiles", PreferenceConstants.P_EVE_HOME + File.separator + "userfiles");
		}
		return str.replace("/userfiles", PreferenceConstants.P_EVE_HOME + File.separator + "userfiles");
	}
	
	private String loadTemplate() {
		String html = null;
		InputStream in = getClass().getResourceAsStream("/res/template.htm");
		try {
			byte[] b = new byte[in.available()];
			in.read(b);
			html = new String(b, StandardCharsets.UTF_8);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return html;
	}
	
	private String loadSubTemplate() {
		String html = null;
		InputStream in = getClass().getResourceAsStream("/res/sub.htm");
		try {
			byte[] b = new byte[in.available()];
			in.read(b);
			html = new String(b, StandardCharsets.UTF_8);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return html;
	}
	
	private void save(Path tmp, String html) {
		tmp = DataManager.getInstance().getTempPath();
		OutputStream outputStream = null;
		try {
			outputStream = new FileOutputStream(tmp.toString() + File.separator + name);
			outputStream.write(html.getBytes());
			outputStream.flush();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally{
		
			if(outputStream != null) {
				try {
					outputStream.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			outputStream = null;
		}
	}
	
	private void batchJQuery(Path tmp) {
		InputStream in = getClass().getResourceAsStream("/res/jslib.tar.gz"); // TODO

		try {
			byte[] b = new byte[in.available()];
			for(int offset=0,n=0; offset < b.length; offset+=n) {
				n = in.read(b, offset, b.length - offset);
			}
			DataManager.extractGZipTo(b, tmp.toFile());
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		finally {
			if(in != null) {
				try {
					in.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				in = null;
			}
		}
	}
}