package com.halkamalka.ever.eve.preferences;

import java.io.File;

import com.halkamalka.ever.eve.Activator;

/**
 * Constant definitions for plug-in preferences
 */
public class PreferenceConstants {

	public static final String P_PATH = "pathPreference";

	public static final String P_BOOLEAN = "booleanPreference";

	public static final String P_CHOICE = "choicePreference";

	public static final String P_STRING = "stringPreference";
	
	public static final String P_SERVER_IP = "serverIpPreference";
	
	public static final String P_SERVER_PORT = "serverPortPreference";
	
	public static final String P_DB_NAME = "product.db";
	
	public static final String P_GZIP_IMG_NAME = "userfiles.tar.gz";
	
	public static final String P_EVE_HOME = System.getProperty("user.home") + File.separator + ".eve";
	
//	public static final String P_HTTP_SERVER_URL = "dbUrlPreference";

	public static String getDBFileURI() {
		return "http://" + Activator.getDefault().getPreferenceStore().getString(PreferenceConstants.P_SERVER_IP) +
				":" +
				 Activator.getDefault().getPreferenceStore().getString(PreferenceConstants.P_SERVER_PORT) +  "/"  + P_DB_NAME;
	}
	
	public static String getImgFileURI() {
		return "http://" + Activator.getDefault().getPreferenceStore().getString(PreferenceConstants.P_SERVER_IP) +
				":" +
				 Activator.getDefault().getPreferenceStore().getString(PreferenceConstants.P_SERVER_PORT) +  "/"  + P_GZIP_IMG_NAME;
	}
	
	public static String getWebsockURI() {
		return "ws://" + Activator.getDefault().getPreferenceStore().getString(PreferenceConstants.P_SERVER_IP) +
				":" +
				 Activator.getDefault().getPreferenceStore().getString(PreferenceConstants.P_SERVER_PORT);
	}
}
