package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

public class Data양자 extends Data{

	final private static Logger log = Logger.getLogger(Data양자.class.getName());
	
	public Data양자 (String name, String path) {
		super(name, path);
	}

	@Override
	public void parse() throws Exception {
		// TODO Auto-generated method stub
		
		File file = new File(path);
//		log.info(">>>>>>>> " + file.toString());
		Document doc = Jsoup.parse(file,  "ks_c_5601-1987", file.toURI().toString());
		
		Elements td = doc.select("td.td");
		
		Elements Td = td.select("[align=middle][rowspan]");
		Elements Td1 = td.select("[align=left][rowspan]");
		
		Integer a=0;
		
		for(a=0; a<Td.size(); a++) {
			
			//name
			System.out.println(Td.get(a).text());
			String name =Td.get(a).text();
			Data data = DataManager.getInstance().findByName(name);

			//recommend
			System.out.println(Td1.get(a).text());
			String prescription = Td1.get(a).text();
			if(org.apache.commons.exec.OS.isFamilyWindows()) {
				data.setPrescription(prescription);
			}
			else {
				data.setPrescription(prescription);
			}

			// TODO : notify to UI.
		}
	}
}
