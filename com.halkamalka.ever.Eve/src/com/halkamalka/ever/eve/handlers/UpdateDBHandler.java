package com.halkamalka.ever.eve.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.halkamalka.ever.eve.core.data.DataManager;

public class UpdateDBHandler extends AbstractHandler {

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
//		IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
////		BusyIndicator.showWhile(display, runnable); 
//		window.getShell().getDisplay().asyncExec(new Runnable(){
//			@Override
//			public void run() {
//				DataManager.getInstance().downloadImage();
//				DataManager.getInstance().downloadDB();
//			}
//		});
		Job job = new Job("db update") {
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				// TODO Auto-generated method stub
				monitor.beginTask("start task", 100);
				monitor.subTask("update images");
				DataManager.getInstance().downloadImage();
				monitor.worked(50);
				monitor.subTask("update database");
				DataManager.getInstance().downloadDB();
				monitor.worked(50);
				return Status.OK_STATUS;
			}
		};
		job.schedule();
		
		return null;
	}
}