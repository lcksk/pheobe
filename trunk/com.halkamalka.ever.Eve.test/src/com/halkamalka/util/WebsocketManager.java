package com.halkamalka.util;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import org.eclipse.jetty.websocket.api.Session;
import org.eclipse.jetty.websocket.api.annotations.OnWebSocketClose;
import org.eclipse.jetty.websocket.api.annotations.OnWebSocketConnect;
import org.eclipse.jetty.websocket.api.annotations.OnWebSocketMessage;
import org.eclipse.jetty.websocket.api.annotations.WebSocket;
import org.eclipse.jetty.websocket.client.WebSocketClient;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

@WebSocket(maxTextMessageSize = 256 * 1024* 1024)
public class WebsocketManager {

	final private static Logger log = Logger.getLogger(WebsocketManager.class.getName());
	private static WebsocketManager manager = null;
	public static WebsocketManager getInstance(){
		if(manager == null){
			manager = new WebsocketManager();
		}
		return manager;
	}
	
	
    private final CountDownLatch closeLatch;
    private final WebSocketClient client;
    private ArrayList<WebsocketListener> listener = new ArrayList<WebsocketListener>();
   
    @SuppressWarnings("unused")
    private Session session;
    private boolean connected = false;
    
	private WebsocketManager() {
        this.closeLatch = new CountDownLatch(1);
        client = new WebSocketClient();
	}
	
	
	public boolean connect(URI uri) {
		if(connected) {
			log.warning("already connected");
			return true;
		}
		
		try {
			client.start();
//			ClientUpgradeRequest request = new ClientUpgradeRequest();
//			client.connect(this, uri, request);
			client.connect(this, uri);
			log.info("Connecting to : " + uri);
			awaitClose(5, TimeUnit.SECONDS);
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
	}
	
	public boolean connect(String uri) throws URISyntaxException {
		return connect(new URI(uri));
	}
	
    public boolean awaitClose(int duration, TimeUnit unit) throws InterruptedException {
        return this.closeLatch.await(duration, unit);
    }
 
    @OnWebSocketClose
    public void onClose(int statusCode, String reason) {
    	fireClosed(session);
        System.out.printf("Connection closed: %d - %s%n", statusCode, reason);
        connected = true;
        this.session = null;
        this.closeLatch.countDown();
    }
 
    @OnWebSocketConnect
    public void onConnect(Session session) {
    	log.info("Got connect: " +  session.toString());
        connected = true;
        this.session = session;
        fireConnected(session);
//        try {
//            Future<Void> fut;
//            fut = session.getRemote().sendStringByFuture("Hello");
//            fut.get(2, TimeUnit.SECONDS);
//            fut = session.getRemote().sendStringByFuture("Thanks for the conversation.");
//            fut.get(2, TimeUnit.SECONDS);
//            session.close(StatusCode.NORMAL, "I'm done");
//        } catch (Throwable t) {
//            t.printStackTrace();
//        }
    }
 
    @OnWebSocketMessage
    public void onMessage(String msg) {
        JSONParser parser = new JSONParser();
        try {
        	Object o = parser.parse(msg);
        	if(o instanceof JSONObject) {
            	JSONObject json = (JSONObject) parser.parse(msg);
            	fireMessage(json);
        		return;
        	}
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
   }
    
    public void close() {
    	try {
			client.stop();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    public void send(JSONObject o) {
    	try {
    		log.info(o.toString());
			this.session.getRemote().sendString(o.toString());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    public void test_send() {
    	JSONObject o = new JSONObject();
//    	{"jsonrpc":"2.0","method":"merchant_check","params":{"hostID":150999,"orderID":107,"amount":"7777","currency":"051","mid":15001038,"tid":15531038,"mtpass":"12345","trxnDetails":""},"id":107}
    	o.put("jsonrpc", "2.0");
    	o.put("method", "system::upload");
    	o.put("id", "10");
    	try {
    		log.info(o.toString());
			this.session.getRemote().sendString(o.toString());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    private void fireMessage(JSONObject o) {
    	for(Iterator<WebsocketListener> it = listener.iterator(); it.hasNext();) {
    		WebsocketListener target = it.next();
    		target.onMessage(o);
    	}
    }
    
    private void fireConnected(Session o) {
    	for(Iterator<WebsocketListener> it = listener.iterator(); it.hasNext();) {
    		WebsocketListener target = it.next();
    		target.onConnect(o);
    	}
    }
    
    private void fireClosed(Session o) {
    	for(Iterator<WebsocketListener> it = listener.iterator(); it.hasNext();) {
    		WebsocketListener target = it.next();
    		target.onClose(o);
    	}
    }
    
    public void addWebsocketListener(WebsocketListener l) {
    	listener.add(l);
    }
    
    public void removeWebsocketListener(WebsocketListener l) {
    	listener.remove(l);
    }
}
