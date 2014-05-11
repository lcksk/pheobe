package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;
import java.util.logging.Logger;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

public abstract class Data implements HtmlParserable {

	final private static Logger log = Logger.getLogger(Data.class.getName());

	public static final Integer LOWEST = new Integer(-3);
	public static final Integer LOWER = new Integer(-2);
	public static final Integer LOW = new Integer(-1);
	public static final Integer NORMAL = new Integer(0);
	public static final Integer HIGH = new Integer(1);
	public static final Integer HIGHER = new Integer(2);
	public static final Integer HIGHEST = new Integer(3);
	
	protected String name = null;
	protected String path = null;
	protected String prescription = null;
	protected Map<String, DataItem> map = null;
	
	public Data(String name, String path) {
		this.name = name;
		this.path = path;
		this.map = new HashMap<String, DataItem>();
	}
	
	public Map<String, DataItem> getMap() {
		return map;
	}

	public String getName() {
		return name;
	}
	
	public String getPath() {
		return path;
	}
	
	//TODO
	public String getPrescriptionPath() {
		return path.substring(0, path.indexOf(".htm")) + "_pres.htm";
	}

	public String getPrescription() {
		return prescription;
	}

	public void setPrescription(String prescription) {
		this.prescription = prescription;
	}
	
	public DataItem get(String key) {
		return map.get(key);
	}

	public void set(String key, DataItem value){
		map.put(key, value);
	}
	
	private float[] parseRange(String value) {
		float[] range = new float[2];
		Scanner scanner = new Scanner (value);
		for(int k = 0; scanner.hasNext(); k++) {
			String tmp = scanner.next();
			switch(k) {
			case 0:
				range[0] = Float.parseFloat(tmp);
				break;
				
			case 2:
				range[1] = Float.parseFloat(tmp);
				break;
			}
		}
		scanner.close();
		return range;
	}
	
	public void parse() throws Exception {
		// TODO Auto-generated method stub
		File file = new File(path);
		log.info(">>>>>>>> " + file.toString());
//		Document doc = Jsoup.parse(file,  "ks_c_5601-1987", file.toURI().toString());
		byte[] raw = org.apache.commons.io.FileUtils.readFileToByteArray(file);
		Document doc = Jsoup.parse(new String(raw));

		Elements td = doc.select("td.td[align=middle]");
		int i=0;
		int j=0;
		
		for(i=0; i<td.size(); i++) {
			j=i%4;
			if(j==0) {
				String name = td.eq(i).html();
				String image = td.eq(i+3).select("img").attr("src");
				
				String scope = td.eq(i+1).html();
				float[] range = parseRange(scope);
				log.info("range: " + range[0]  + "-" +  range[1]);
				
				String result = td.eq(i+2).html();
				float calibration = Float.parseFloat(result);

				Integer status = resolve(image);
				set(name, new DataItem(name, range, calibration, status));
			}
		}
		log.info(">>>>>>>> " + map.toString());
	}
	
	public Integer resolve(String image) {

		Integer status = null;
		switch(image) {
		case "YC01.gif":
			status = Data.LOWEST;
			break;
		case "YC02.gif":
			status = Data.LOWER;
			break;
		case "YC03.gif":
			status = Data.LOW;
			break;
		case "YC04.gif":
			status = Data.NORMAL;
			break;
		case "YC05.gif":
			status = Data.HIGH;
			break;
		case "YC06.gif":
			status = Data.HIGHER;
			break;
		case "YC07.gif":
			status = Data.HIGHEST;
			break;
		case "YC09.gif":
			status = Data.LOW;
			break;
		case "YC08.gif":
			status = Data.NORMAL;
			break;
		case "YC10.gif":
			status = Data.HIGH;
			break;
		}
		return status;
	}
	
	
	public boolean hasAbnormal() {
		return (map.size() > 0);
	}
	
	public DataItem[] findAbnormal() {
		
		ArrayList<DataItem> tmp = new ArrayList<DataItem>();
		
		Set<String> set = map.keySet();
		for(Iterator<String> it = set.iterator(); it.hasNext();) {
			String key = it.next();
			DataItem item = map.get(key);
			
			if(!(item.getStatus().equals(Data.NORMAL) || item.getStatus().equals(Data.LOW) || item.getStatus().equals(Data.HIGH))) {
				tmp.add(item);
			}
		}
		return (DataItem[]) tmp.toArray(new DataItem[tmp.size()]);
	}
}
