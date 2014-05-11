package com.halkamalka.util;

import org.eclipse.jetty.websocket.api.Session;
import org.json.simple.JSONObject;

public interface WebsocketListener {
	public void onMessage(JSONObject o);
	public void onConnect(Session session);
	public void onClose(Session session);
}
