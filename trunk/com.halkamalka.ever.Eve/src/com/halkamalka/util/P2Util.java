package com.halkamalka.util;


import java.util.Collection;
import java.util.logging.Logger;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.equinox.internal.p2.core.helpers.LogHelper;
import org.eclipse.equinox.internal.p2.core.helpers.ServiceHelper;
import org.eclipse.equinox.p2.core.IProvisioningAgent;
import org.eclipse.equinox.p2.engine.IProfile;
import org.eclipse.equinox.p2.engine.IProfileRegistry;
import org.eclipse.equinox.p2.metadata.IInstallableUnit;
import org.eclipse.equinox.p2.operations.ProvisioningJob;
import org.eclipse.equinox.p2.operations.ProvisioningSession;
import org.eclipse.equinox.p2.operations.UpdateOperation;
import org.eclipse.equinox.p2.query.QueryUtil;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.PlatformUI;

import com.halkamalka.ever.eve.Activator;

public class P2Util {
    private static final String JUSTUPDATED = "justUpdated";
    final private static Logger log = Logger.getLogger(P2Util.class.getName());
    
    public static void autoupdate(long millisDelay) {
        final IProvisioningAgent agent = (IProvisioningAgent) ServiceHelper.getService(Activator.getDefault()
                .getBundle().getBundleContext(), IProvisioningAgent.SERVICE_NAME);
        if (agent == null) {
            LogHelper.log(new Status(IStatus.ERROR, Activator.PLUGIN_ID,
                    "No provisioning agent found.  This application is not set up for updates."));
        }
        // XXX if we're restarting after updating, don't check again.
        final IPreferenceStore prefStore = Activator.getDefault().getPreferenceStore();
        if (prefStore.getBoolean(JUSTUPDATED)) {
            prefStore.setValue(JUSTUPDATED, false);
            return;
        }
        /*
         * We create a Job to run the update operation with a cancellable
         * dialog.
         */
        final Job job = new Job("Application Update") {

            @Override
            protected IStatus run(IProgressMonitor monitor) {
                IStatus updateStatus = update(agent, monitor);
                return updateStatus;
            }
        };
        job.setUser(true);
        /*
         * Hook after-update todos as a listener.
         */
        job.addJobChangeListener(new JobChangeAdapter() {
            public void done(IJobChangeEvent event) {
                IStatus updateStatus = event.getResult();
                if (updateStatus.getCode() == UpdateOperation.STATUS_NOTHING_TO_UPDATE) {
                    // do nothing..
                	log.info("nothing to update");
                }
                else if (updateStatus.getSeverity() != IStatus.ERROR) {
                    prefStore.setValue(JUSTUPDATED, true);
                    Display.getDefault().syncExec(new Runnable() {

                        @Override
                        public void run() {
                            PlatformUI.getWorkbench().restart();
                        }
                    });
                }
                else {
                    log.info(updateStatus.getMessage());
                }
            }
        });
        job.schedule(millisDelay);

    }

    private static IStatus update(IProvisioningAgent agent, IProgressMonitor monitor) {
        UpdateOperation operation = null;
        /*
         * Any update has to be done against a profile. We'll use the first
         * profile in profile registry under which all the installable units are
         * registered. It's a self hosting profile.
         */
        IProfileRegistry registry = (IProfileRegistry) agent.getService(IProfileRegistry.SERVICE_NAME);
        IProfile[] profiles = registry.getProfiles();
        IProfile profile = null;
        if (profiles.length > 0) {
            profile = profiles[0];
        }
        else {
            return new Status(IStatus.ERROR, "", "No profile found to update!");
        }

        /*
         * Query to find all installable units in the profile. These will be
         * searched in the update site for updates.
         */
        Collection<IInstallableUnit> toUpdate = profile.query(
QueryUtil.createIUAnyQuery(),
            monitor).toSet();
        ProvisioningSession provisioningSession = new ProvisioningSession(agent);
        operation = new UpdateOperation(provisioningSession, toUpdate);
        operation.setProfileId(profile.getProfileId());
        SubMonitor sub = SubMonitor.convert(monitor, "Checking for Application updates...", 200);
        IStatus status = operation.resolveModal(sub.newChild(100));
        if (status.getCode() == UpdateOperation.STATUS_NOTHING_TO_UPDATE) {
            return status;
        }
        if (status.getSeverity() == IStatus.CANCEL)
            throw new OperationCanceledException();

        if (status.getSeverity() != IStatus.ERROR) {
            // More complex status handling might include showing the user what
            // updates
            // are available if there are multiples, differentiating patches vs.
            // updates, etc.
            // We simply update as suggested by the operation without any user
            // interaction.
            sub.setTaskName("Updating Application to latest version available..");
            ProvisioningJob job = operation.getProvisioningJob(sub);
            status = job.runModal(sub.newChild(100));
            if (status.getSeverity() == IStatus.CANCEL)
                throw new OperationCanceledException();
        }
        return status;

    }

}