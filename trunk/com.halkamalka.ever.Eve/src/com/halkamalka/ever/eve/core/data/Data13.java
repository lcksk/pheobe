package com.halkamalka.ever.eve.core.data;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Set;

public class Data13 extends Data {

	public Data13(String path) {
		super(path);
		// TODO Auto-generated constructor stub
	}

	public DataItem[] findAbnormal() {
		
		ArrayList<DataItem> tmp = new ArrayList<DataItem>();
		
		Set<String> set = map.keySet();
		for(Iterator<String> it = set.iterator(); it.hasNext();) {
			String key = it.next();
			DataItem item = map.get(key);
			
			if(item.getName().startsWith("ph")) {
				if(!(item.getStatus().equals(Data.NORMAL))) {
						tmp.add(item);
				}
			}
			else {
				if(!(item.getStatus().equals(Data.NORMAL) || item.getStatus().equals(Data.LOW) || item.getStatus().equals(Data.HIGH))) {
						tmp.add(item);
				}
			}
		}
		return (DataItem[]) tmp.toArray(new DataItem[tmp.size()]);
	}
}
