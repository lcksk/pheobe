package com.halkamalka.ever.eve.handlers;

import java.util.logging.Logger;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.handlers.HandlerUtil;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.ever.eve.core.data.Product;
import com.halkamalka.ever.eve.views.CategoryViewPart;

public class OpenFileHandler extends AbstractHandler {
	
	private String path = null;
	/**
	 * The constructor.
	 */
	final private static Logger log = Logger.getLogger(OpenFileHandler.class.getName());
	public OpenFileHandler() {
	}

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {
		IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);

		FileDialog dialog = new FileDialog(window.getShell(), SWT.OPEN);
		dialog.setFilterExtensions(new String[] {"*.zip", "*.ZIP"});
		path = dialog.open();
		if(path == null){
			MessageDialog.openInformation(
					window.getShell(),
					"ever & ever",
					"Select an appropriate file.");
			return null;
		}


		DataManager.getInstance().reload(path);
		
//		Job job = new Job("load data") {
//			@Override
//			protected IStatus run(IProgressMonitor monitor) {
////				monitor.beginTask("start task", 100);
////				monitor.subTask("loading all products");
////				Product[] product = DataManager.getInstance().getProducts();
////				for(int i = 0; i < product.length; i++) {
////					System.out.println(product[i].getDescription());
////					monitor.worked(100/product.length);
////					monitor.subTask(product[i].getMajor());
////				}
//				log.info(OpenFileHandler.this.path);
//				monitor.beginTask("start task", 100);
//				monitor.subTask("loading results");
//				DataManager.getInstance().reload(OpenFileHandler.this.path);
//				monitor.worked(100);
//				monitor.subTask("loading completed");
//				return Status.OK_STATUS;
//			}
//		};
//		job.schedule();
		
		return null;
	}
}
