package com.halkamalka.ever.eve.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import com.halkamalka.ever.eve.Activator;

/**
 * Class used to initialize default preference values.
 */
public class PreferenceInitializer extends AbstractPreferenceInitializer {

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer#initializeDefaultPreferences()
	 */
	public void initializeDefaultPreferences() {
		IPreferenceStore store = Activator.getDefault().getPreferenceStore();
		store.setDefault(PreferenceConstants.P_BOOLEAN, true);
		store.setDefault(PreferenceConstants.P_CHOICE, "choice2");
		store.setDefault(PreferenceConstants.P_STRING,"Default value");
//		store.setDefault(PreferenceConstants.P_HTTP_SERVER_URL,"http://14.33.234.132:8080");
		store.setDefault(PreferenceConstants.P_SERVER_IP,"14.33.234.132");
		store.setDefault(PreferenceConstants.P_SERVER_PORT,"8080");
	}
}
