package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.util.logging.Logger;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

public class Data99 extends Data{

	final private static Logger log = Logger.getLogger(Data99.class.getName());

	public Data99(String name, String path) {
		super(name, path);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void parse() throws Exception {
		// TODO Auto-generated method stub
		File file = new File(path);
		log.info(">>>>>>>> " + file.toString());
		Document doc = Jsoup.parse(file,  "ks_c_5601-1987", file.toURI().toString());
		Elements table = doc.select("table");
		log.info(table.text());
	}
}
