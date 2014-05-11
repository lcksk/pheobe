package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Iterator;
import java.util.Set;
import java.util.SortedMap;
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
//		Document doc = Jsoup.parse(file,  "ks_c_5601-1987", file.toURI().toString());
		byte[] raw = org.apache.commons.io.FileUtils.readFileToByteArray(file);
//		Document doc = Jsoup.parse(file,  "ksc5601", file.toURI().toString());
		Document doc = Jsoup.parse(new String(raw));
		
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
//				SortedMap<String, Charset> charsets = Charset.availableCharsets();
//				Set<String> keys = charsets.keySet();
//				for(Iterator<String> it = keys.iterator(); it.hasNext();) {
//					String key = it.next();
//					Charset charset = charsets.get(key);
//					log.info(key + charset.toString());
//					log.info(new String(prescription.getBytes(), charset));
//				}
				prescription = new String(prescription.getBytes(), Charset.forName("EUC-KR"));
				data.setPrescription(prescription);
			}
			else {
				data.setPrescription(prescription);
			}

			// TODO : notify to UI.
		}
	}
}
