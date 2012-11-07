/*
 * xwifi_helper.c
 *
 *  Created on: Oct 18, 2012
 *      Author: buttonfly
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "os.h"
#include "nds/xwifi.h"
#include "xwifi_wpa_priv.h"
#include "xwifi_dbg.h"

const char* xwifi_get_default_ifname()
{
	S_;
	char *ifname = NULL;

	struct dirent *dent;
	DIR *dir = opendir(CONFIG_CTRL_IFACE_DIR);
	if (!dir) {
		return NULL;
	}
	while ((dent = readdir(dir))) {
#ifdef _DIRENT_HAVE_D_TYPE
		/*
		 * Skip the file if it is not a socket. Also accept
		 * DT_UNKNOWN (0) in case the C library or underlying
		 * file system does not support d_type.
		 */
		if (dent->d_type != DT_SOCK && dent->d_type != DT_UNKNOWN)
			continue;
#endif /* _DIRENT_HAVE_D_TYPE */
		if (os_strcmp(dent->d_name, ".") == 0 ||
		    os_strcmp(dent->d_name, "..") == 0)
			continue;
		printf("Selected interface '%s'\n", dent->d_name);
		ifname = os_strdup(dent->d_name);
		break;
	}
	closedir(dir);

	return ifname;
}
