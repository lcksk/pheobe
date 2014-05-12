package com.halkamalka.ever.eve.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.ever.eve.core.data.Product;

public class ListProductHandler extends AbstractHandler {

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {

		Job job = new Job("db update") {
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				monitor.beginTask("start task", 100);
				monitor.subTask("loading all products");
				Product[] product = DataManager.getInstance().getProducts();
				for(int i = 0; i < product.length; i++) {
					System.out.println(product[i].getDescription());
					monitor.worked(100/product.length);
					monitor.subTask(product[i].getMajor());
					try {
						Thread.sleep(100);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
				return Status.OK_STATUS;
			}
		};
		job.schedule();
		return null;
	}
}
