package com.halkamalka.ever.eve.handlers;

import java.net.URISyntaxException;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.halkamalka.ever.eve.preferences.PreferenceConstants;
import com.halkamalka.util.WebsocketManager;

public class ConnectHandler extends AbstractHandler {

	private final WebsocketManager client = WebsocketManager.getInstance();
	
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {

		if(client.isConnected()) {
			Job job = new Job("disconnect") {
				@Override
				protected IStatus run(IProgressMonitor monitor) {
					monitor.beginTask("start task", 100);
					monitor.subTask("disconnect...");
					client.close();

					monitor.worked(100);
					monitor.subTask("loading completed");
					return Status.OK_STATUS;

				}
			};
			job.schedule();
		}
		else {
			Job job = new Job("connect") {
				@Override
				protected IStatus run(IProgressMonitor monitor) {
					monitor.beginTask("start task", 100);
					monitor.subTask("connect...");
					try {
						client.connect(PreferenceConstants.getWebsockURI());
					} catch (URISyntaxException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					monitor.worked(100);
					monitor.subTask("loading completed");
					return Status.OK_STATUS;

				}
			};
			job.schedule();
		}

		return null;
	}

}
