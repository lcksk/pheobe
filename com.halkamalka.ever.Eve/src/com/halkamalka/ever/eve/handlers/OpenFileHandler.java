package com.halkamalka.ever.eve.handlers;

import java.util.logging.Logger;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.handlers.HandlerUtil;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.ever.eve.views.CategoryViewPart;

public class OpenFileHandler extends AbstractHandler {
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
		String path = dialog.open();
		if(path == null){
			MessageDialog.openInformation(
					window.getShell(),
					"ever & ever",
					"Select an appropriate file.");
			return null;
		}

		log.info(path);
		DataManager.getInstance().reload(path);
		return null;
	}
}
