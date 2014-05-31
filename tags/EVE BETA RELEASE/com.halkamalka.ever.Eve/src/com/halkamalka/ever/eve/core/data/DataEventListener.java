package com.halkamalka.ever.eve.core.data;


public interface DataEventListener {
	public void dataAdded(DataEventObject e);
	public void dataRemoved(DataEventObject e);
	public void dataReset();
	public void dataLoadCompleted();
}
