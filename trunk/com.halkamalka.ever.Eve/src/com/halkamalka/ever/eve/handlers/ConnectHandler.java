package com.halkamalka.ever.eve.handlers;

import java.net.URISyntaxException;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jetty.websocket.api.Session;
import org.json.simple.JSONObject;

import com.halkamalka.ever.eve.preferences.PreferenceConstants;
import com.halkamalka.util.WebsocketListener;
import com.halkamalka.util.WebsocketManager;

public class ConnectHandler extends AbstractHandler implements WebsocketListener {

	private final WebsocketManager client = WebsocketManager.getInstance();
	
	public ConnectHandler() {
		System.out.println("called every time??");
		client.addWebsocketListener(this);
	}
	
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {

		Job job = new Job("connect") {
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				monitor.beginTask("start task", 100);
				monitor.subTask("connect...");
				monitor.worked(50);
				try {
					client.connect(PreferenceConstants.getWebsockURI());
				}
				catch (URISyntaxException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				monitor.worked(50);
				monitor.subTask("connected");
				try {
					Thread.sleep(100);
				}
				catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				return Status.OK_STATUS;

			}
		};
		job.schedule();
//		if(client.isConnected()) {
//			Job job = new Job("disconnect") {
//				@Override
//				protected IStatus run(IProgressMonitor monitor) {
//					monitor.beginTask("start task", 100);
//					monitor.subTask("disconnect...");
//					client.close();
//					try {
//						Thread.sleep(100);
//					} 
//					catch (InterruptedException e) {
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
//					monitor.worked(100);
//					monitor.subTask("disconnected.");
//					return Status.OK_STATUS;
//
//				}
//			};
//			job.schedule();
//		}
//		else {
//			Job job = new Job("connect") {
//				@Override
//				protected IStatus run(IProgressMonitor monitor) {
//					monitor.beginTask("start task", 100);
//					monitor.subTask("connect...");
//					monitor.worked(50);
//					try {
//						client.connect(PreferenceConstants.getWebsockURI());
//					}
//					catch (URISyntaxException e) {
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
//					monitor.worked(50);
//					monitor.subTask("connected");
//					try {
//						Thread.sleep(100);
//					}
//					catch (InterruptedException e) {
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
//					return Status.OK_STATUS;
//
//				}
//			};
//			job.schedule();
//		}

		return null;
	}

	@Override
	public void onMessage(JSONObject o) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onConnect(Session session) {
		// TODO Auto-generated method stub
		setBaseEnabled(false);
	}

	@Override
	public void onClose(Session session) {
		// TODO Auto-generated method stub
		setBaseEnabled(true);
	}

}
