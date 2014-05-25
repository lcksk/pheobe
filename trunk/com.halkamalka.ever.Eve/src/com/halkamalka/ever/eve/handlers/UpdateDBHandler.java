package com.halkamalka.ever.eve.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jetty.websocket.api.Session;
import org.eclipse.ui.PlatformUI;
import org.json.simple.JSONObject;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.ever.eve.views.NotificationPopUpUI;
import com.halkamalka.util.WebsocketListener;
import com.halkamalka.util.WebsocketManager;

public class UpdateDBHandler extends AbstractHandler  implements WebsocketListener {

	private final WebsocketManager client = WebsocketManager.getInstance();
	
	public UpdateDBHandler() {
		client.addWebsocketListener(this);
	}
	
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		Job job = new Job("db update") {
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				// TODO Auto-generated method stub
				monitor.beginTask("start task", 100);
				monitor.subTask("update images");
				DataManager.getInstance().downloadImage();
				monitor.worked(50);
//				try {
//					Thread.sleep(100);
//				} catch (InterruptedException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
				monitor.subTask("update database");
				DataManager.getInstance().downloadDB();
				monitor.worked(30);
//				try {
//					Thread.sleep(100);
//				} catch (InterruptedException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
				return Status.OK_STATUS;
			}
		};
		job.schedule();
		
		return null;
	}
	
	
	@Override
	public void onMessage(JSONObject o) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onConnect(Session session) {
		// TODO Auto-generated method stub
		setBaseEnabled(true);
	}

	@Override
	public void onClose(Session session) {
		// TODO Auto-generated method stub
		setBaseEnabled(false);
	}
}
