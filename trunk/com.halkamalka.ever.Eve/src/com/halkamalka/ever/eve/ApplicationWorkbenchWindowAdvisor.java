package com.halkamalka.ever.eve;

import java.io.IOException;

import org.eclipse.equinox.p2.core.IProvisioningAgent;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;
import org.eclipse.ui.application.IWorkbenchWindowConfigurer;
import org.eclipse.ui.application.WorkbenchWindowAdvisor;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.util.P2Util;
import com.halkamalka.util.UpdateJob;

public class ApplicationWorkbenchWindowAdvisor extends WorkbenchWindowAdvisor {

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
    
    
    @Override
    public void postWindowOpen() {
        BundleContext bundleContext = Activator.getDefault().getBundle().getBundleContext();
        ServiceReference<IProvisioningAgent> serviceReference =   bundleContext.getServiceReference(IProvisioningAgent.class);
        IProvisioningAgent agent = bundleContext.getService(serviceReference);
        if (agent == null) {
            System.out.println(">> no agent loaded!");
            return;
        }
        // Adding the repositories to explore
        if (! P2Util.addRepository(agent, "http://halkamalka.com/p2/eve/repository")) {
            System.out.println(">> could no add repostory!");
            return;
        }
        // scheduling job for updates
        UpdateJob updateJob = new UpdateJob(agent);
        updateJob.schedule();
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
