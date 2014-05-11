package com.halkamalka.ever.eve.core.data;

import java.util.EventObject;
import java.util.logging.Logger;

public class DataEventObject extends EventObject {

	final private static Logger log = Logger.getLogger(DataEventObject.class.getName());
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public DataEventObject(Object source) {
		super(source);
		// TODO Auto-generated constructor stub
		if(!(source instanceof Data)) {
			log.warning("wrong type!");
		}
	}

}
