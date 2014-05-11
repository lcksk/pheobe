package com.halkamalka.ever.eve.core.data;


public interface DataEventSource {
	public void addDataEventListener(DataEventListener listener);
	public void removeDataEventListener(DataEventListener listener);
	public void fireDataEvent(DataStatus stat, Data data);
}
