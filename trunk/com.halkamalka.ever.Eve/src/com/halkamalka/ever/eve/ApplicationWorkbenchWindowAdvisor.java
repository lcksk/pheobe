package com.halkamalka.ever.eve;

import java.io.IOException;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.equinox.internal.p2.core.helpers.LogHelper;
import org.eclipse.equinox.internal.p2.core.helpers.ServiceHelper;
import org.eclipse.equinox.p2.core.IProvisioningAgent;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;
import org.eclipse.ui.application.IWorkbenchWindowConfigurer;
import org.eclipse.ui.application.WorkbenchWindowAdvisor;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.util.P2Util;

public class ApplicationWorkbenchWindowAdvisor extends WorkbenchWindowAdvisor {

	private static final String JUSTUPDATED = "justUpdated";
	IProvisioningAgent agent;
	
    public ApplicationWorkbenchWindowAdvisor(IWorkbenchWindowConfigurer configurer) {
        super(configurer);
    }

    public ActionBarAdvisor createActionBarAdvisor(IActionBarConfigurer configurer) {
        return new ApplicationActionBarAdvisor(configurer);
    }

    @Override
    public void preWindowOpen() {
        IWorkbenchWindowConfigurer configurer = getWindowConfigurer();
//        configurer.setInitialSize(new Point(400, 300));
        configurer.setShowCoolBar(true);
        configurer.setShowStatusLine(true);
        configurer.setShowProgressIndicator(true);
        configurer.setTitle("EVER&EVER - http://www.evernever.co.kr"); //$NON-NLS-1$
    }

    /*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.ui.application.WorkbenchWindowAdvisor#postWindowOpen()
	 */
    @Override
	public void postWindowOpen() {
//        BundleContext bundleContext = Activator.getDefault().getBundle().getBundleContext();
//        ServiceReference<IProvisioningAgent> serviceReference =   bundleContext.getServiceReference(IProvisioningAgent.class);
//        IProvisioningAgent agent = bundleContext.getService(serviceReference);
//        if (agent == null) {
//            System.out.println(">> no agent loaded!");
//            return;
//        }
//        // Adding the repositories to explore
//        if (! P2Util.addRepository(agent, "http://halkamalka.com/p2/eve/repository")) {
//            System.out.println(">> could no add repostory!");
//            return;
//        }
//		final IProvisioningAgent agent = (IProvisioningAgent) ServiceHelper
//				.getService(Activator.bundleContext,
//						IProvisioningAgent.SERVICE_NAME);
//		if (agent == null) {
//			LogHelper
//					.log(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
//							"No provisioning agent found.  This application is not set up for updates."));
//		}
    	P2Util.autoupdate(0);
        
	}
    
    @Override
    public void dispose() {
    	try {
			DataManager.getInstance().close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	super.dispose();
    }
}
