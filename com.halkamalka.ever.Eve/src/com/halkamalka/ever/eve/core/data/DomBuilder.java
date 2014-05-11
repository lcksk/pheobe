package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Path;

import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.swt.internal.gtk.OS;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import com.halkamalka.ever.eve.views.BrowserViewPart;

public class DomBuilder {
	
	Document html;
	String name;

	public DomBuilder(String name) {
		// TODO Auto-generated constructor stub
		this.name = name;
	}
	
	public void build(Data data, Product[] product) throws ParserConfigurationException {
		String html = loadTemplate();
		Document doc = Jsoup.parse(html);
		
//		Element body = html.createElement("body");

		if(org.apache.commons.exec.OS.isFamilyWindows()) {
			// replace encoding to euc-kr
			Element meta = doc.select("meta").first();
			meta.attr("content", "text/html; charset=euc-kr");
		}
		
		System.out.println(html.toString());

		Product p = product[0];
		Element majorId = doc.select("#majorId").first();
		majorId.text(p.getMajor());
		
		Element minorId = doc.select("#minorId").first();
		minorId.text(p.getMinor());
		
		Element scope = doc.select("#scope").first();
		scope.text("fdasfdasfdas");
		
		Element value = doc.select("#value").first();
		value.text("asdd");
		
		Element result = doc.select("#result").first();
		if(data.getPrescription() != null) {
			result.text(data.getPrescription());
		}
		
		Element productImage = doc.select("#productImage").first();
//		productImage.attr("src", "data:image/*;base64,"+p.getImage());
		productImage.attr("src", p.getImage());
		productImage.html("<p>lorem ipsum</p>");

		Element intro = doc.select("#productDescription").first();
		intro.html(replacePath(p.getDescription()));

		Path tmp = DataManager.getInstance().getTempPath();
		
		System.out.println(doc.html());
		
		OutputStream outputStream = null;
		try {
			outputStream = new FileOutputStream(tmp.toString() + File.separator + name);
			outputStream.write(doc.html().getBytes());
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

	private String replacePath(String str) {
		return str.replaceAll("/userfiles", System.getProperty("user.home") + File.separator + ".eve" + File.separator + "userfiles");
	}
	
	private String loadTemplate() {
		String html = null;
		InputStream in = getClass().getResourceAsStream("/res/template.htm");
		try {
			byte[] b = new byte[in.available()];
			in.read(b);
			html = new String(b);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return html;
	}
}
