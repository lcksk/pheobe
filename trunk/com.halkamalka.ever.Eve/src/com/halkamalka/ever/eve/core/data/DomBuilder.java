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
		// TODO Auto-generated constructor stub
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
//		Element body = html.createElement("body");

		if(org.apache.commons.exec.OS.isFamilyWindows()) {
			// replace encoding to euc-kr
			Element meta = doc.select("meta").first();
			meta.attr("content", "text/html; charset=euc-kr");
		}
		
//		System.out.println(html.toString());
		Element result = doc.select("#result").first();
		if(data.getPrescription() != null) {
			result.text(data.getPrescription());
		}

		Element majorId = doc.select("#majorId").first();
		majorId.text(data.getName());
		
		for(int i = 0; i < bound.length; i++) {
			
			Bound b = bound[i];

			log.info("@@@@@@@@@@@@@@@@@@@@ " + b.getMajor());
			
//			Product[] products_ = DataManager.getInstance().getProducts(b.getMajor(), null);
			Product[] products_ = DataManager.getInstance().getProducts(b.getName(), null);
			for(int j = 0; j < products_.length; j++) {
				Product product = products_[j];
				log.info("@@@@@@@@@@@  " + product.getName() + "num : " + products_.length);
				Element intro = sub.select("#productDescription").first();
				intro.html(replacePath(product.getDescription()));

				Element products = doc.select("#products").first();
				products.after(sub.html());
			}
			
			Element minorId = doc.select("#minorId").first();
//			minorId.text(p.getMinor());
			
			Element scope = doc.select("#scope").first();
			
			Element value = doc.select("#value").first();
//			value.text("asdd");
			

			
			Element productImage = sub.select("#productImage").first();
//			productImage.attr("src", "data:image/*;base64,"+p.getImage());
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
//		InputStream in = getClass().getResourceAsStream("/res/jQuery.zip"); // TODO
//		OutputStream out = null;
//		try {
//			byte[] b = new byte[in.available()];
//			in.read(b);
//			out = new FileOutputStream(tmp.toString() + File.separator + "..."); // TODO
//			out.write(b);
//			out.flush();
//		} 
//		catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		finally {
//			if(in != null) {
//				try {
//					in.close();
//				} catch (IOException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
//				in = null;
//			}
//			
//			if(out != null) {
//				try {
//					out.close();
//				} catch (IOException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
//				out = null;
//			}
//		}
	}
}
