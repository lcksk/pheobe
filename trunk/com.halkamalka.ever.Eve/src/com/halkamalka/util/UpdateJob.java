//package com.halkamalka.util;
//
//import java.lang.reflect.InvocationTargetException;
//import java.util.logging.Logger;
//
//import org.eclipse.core.runtime.IProgressMonitor;
//import org.eclipse.core.runtime.IStatus;
//import org.eclipse.core.runtime.Status;
//import org.eclipse.core.runtime.jobs.Job;
//import org.eclipse.equinox.p2.core.IProvisioningAgent;
//import org.eclipse.equinox.p2.operations.UpdateOperation;
//import org.eclipse.jface.dialogs.MessageDialog;
//import org.eclipse.jface.dialogs.ProgressMonitorDialog;
//import org.eclipse.jface.operation.IRunnableWithProgress;
//import org.eclipse.swt.widgets.Display;
//import org.eclipse.ui.PlatformUI;
//import org.eclipse.ui.statushandlers.StatusManager;
//
//import com.halkamalka.ever.eve.Activator;
//
//
//
///**
// * A Basic Update Job.
// * 
// * @author mahieddine.ichir@free.fr
// */
//public class UpdateJob extends Job {
//
//	final private static Logger log = Logger.getLogger(UpdateJob.class.getName());
//	
//	private IProvisioningAgent agent;
//	private IStatus result;
//
//	/**
//	 * Base constructor.
//	 * @param agent Provisioning agent
//	 */
//	public UpdateJob(IProvisioningAgent agent) {
//		super("Checking for updates");
//		this.agent = agent;
//	}
//
//	@Override
//	protected IStatus run(IProgressMonitor arg0) {
//		result = P2Util.checkForUpdates(agent, arg0);
//		if (result.getCode() == UpdateOperation.STATUS_NOTHING_TO_UPDATE) {
//			popUpInformation("Nothing to update!");
//		} else {
//			installUpdates();
////			installUpdates(arg0);
//		}
//		return result;
//	}
//
//	/**
//	 * Pop up for updates installation and, if accepted, install updates.
//	 */
//	private void installUpdates() {
//		Display.getDefault().syncExec(new Runnable() {
//			@Override
//			public void run() {
//				boolean install = MessageDialog.openQuestion(null, "Updates", "Updates found! install?");
//				if (install) {
//					ProgressMonitorDialog dialog = new ProgressMonitorDialog(Display.getDefault().getActiveShell());
//					try {
//						dialog.run(true, true, new IRunnableWithProgress() {
//
//							@Override
//							public void run(IProgressMonitor monitor)
//									throws InvocationTargetException,
//									InterruptedException {
//								// TODO Auto-generated method stub
//								P2Util.installUpdates(agent, monitor);
//								PlatformUI.getWorkbench().restart();
//								
//							}
////							@Override
////							public void run(IProgressMonitor arg0) throws InvocationTargetException,
////							InterruptedException {
////								P2Util.installUpdates(agent, arg0);
////								PlatformUI.getWorkbench().restart();
////							}
//						});
//					} catch (Exception e) {
//						e.printStackTrace();
//						result = new Status(Status.ERROR, Activator.PLUGIN_ID, "Update failed!", e);
//						StatusManager.getManager().handle(result);
//					}
//				}
//			}
//		});
//	}
//
//	private void installUpdates(IProgressMonitor arg0) {
//		P2Util.installUpdates(agent, arg0);
//	}
//	
//	/**
//	 * Show a message dialog.
//	 * @param message
//	 */
//	private void popUpInformation(final String message) {
//		Display.getDefault().asyncExec(new Runnable() {
//			@Override
//			public void run() {
//				MessageDialog.openInformation(null, "Updates", message);
//			}
//		});
//	}
//}