package com.halkamalka.ever.eve.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.halkamalka.ever.eve.core.data.DataManager;

public class RefreshHandler extends AbstractHandler{

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {

		Job job = new Job("refresh data") {
			@Override
			protected IStatus run(IProgressMonitor monitor) {

				try{
					DataManager.getInstance().reload();
					monitor.worked(100);
					monitor.subTask("loading completed");
				}
				catch(NullPointerException e) {
					e.printStackTrace();
					monitor.subTask("failed to refresh");
					return Status.CANCEL_STATUS;
				}
				return Status.OK_STATUS;
			}
		};
		job.schedule();
		return null;
		
	}
}
