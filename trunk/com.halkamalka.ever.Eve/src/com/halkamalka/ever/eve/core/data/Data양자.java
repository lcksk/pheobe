package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.io.FileNotFoundException;
import java.nio.charset.Charset;
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
		
		File file = new File(path);
		if(!file.exists()) {
			throw new FileNotFoundException();
		}

		byte[] raw = org.apache.commons.io.FileUtils.readFileToByteArray(file);
		
		if(!Charset.isSupported( "ksc5601")) {
			log.warning("ksc5601 is not supported");
			throw new Exception();
		}
		
		Document doc = Jsoup.parse(new String(raw,  "ksc5601"));
		
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
