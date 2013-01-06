/*
 * xwifi_sys.c
 *
 *  Created on: Dec 3, 2012
 *      Author: buttonfly
 */


#include <unistd.h>
#include <signal.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include <ctype.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/route.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sched.h>
#include <netinet/ether.h>
#include <resolv.h>
#include <stdbool.h>
#include "osl.h"
#include "osldbg.h"

int xwifi_sys_build_args(
    char                *argv[],
    int                 argv_sz,
    char                *pString,
    int                 *pargc)
{
    int     i = 0;

    if ( (pString == NULL) || (argv == NULL) || (pargc == NULL) )
    {
    	ERR("Invalid parameter!");
        return -1;
    }

    while((i < argv_sz) && (*pString != '\0')) {
        argv[i] = pString;
        while ( (*pString != '\0') && !isspace(*pString) )
        {
            pString++;
        }
        if (*pString == '\0') {
            i+=1;
            break;
        }

        while (isspace(*pString)) {
            *pString = '\0';
            pString++;
        }
        i++;
    }

    *pargc = i;
    return 0;
}


int xwifi_sys_system(
    char          *pcCommand,
    char                *pcArgs,
    const char          *pcSearchStr,
    int32_t             lTimeoutMs,
    bool                *pStopCondition,
    char                *pResults,
    uint32_t             ulLength,
    int                 *pChildPid)
{

	int 				tRetCode;
    int             pid     = 0;
    int             pfd[2]  = { 0, 0 };
    int             i;
    int             maxFDs;
    char            *pBuf = NULL;
    char            *argv[20];
    int             argc = 0;

    KASSERT(pcCommand);

    /* Make sure all open fds are set to be closed when exec() is called */
    maxFDs = sysconf(_SC_OPEN_MAX);
    if (maxFDs == -1) {
        maxFDs = 8192;
    }

    for (i = 3; i < maxFDs; ++i) {
        int res = fcntl(i, F_GETFD);
        if (res >= 0)
        {
            fcntl(i, F_SETFD, res | FD_CLOEXEC);
        }
    }


    if (pcSearchStr != NULL)
    {
        /* Create a pipe to capture output */
        if (pipe(pfd) == -1)
        {
        	ERR("%s Pipe failure!", __FUNCTION__);
            return -1;

        }
    }
    memset(argv, 0, sizeof(argv));

    argv[0] = pcCommand;

    /* Prepare the buffer for parsing the arguments */
    if (pcArgs != NULL)
    {
        pBuf = malloc(strlen(pcArgs)+1);
        if (pBuf == NULL)
        {
            ERR("%s(): Ran out of memory parameter!", __FUNCTION__);
            tRetCode = -1;
            goto err_out;
        }
        memcpy(pBuf, pcArgs, strlen(pcArgs)+1);

        if (xwifi_sys_build_args(&argv[1], sizeof(argv)/sizeof(argv[0])-1, pBuf, &argc) != 0)
        {
        	ERR("%s(): Failed to Build argument list! CMD:[%s]", __FUNCTION__, pcArgs);
            tRetCode = EXIT_FAILURE;
            goto err_out;
        }
    }

    pid = vfork();

    /* Child Process */
    if (pid == 0)
    {
        struct sched_param  param;

        M_("%s, Child Process about to Call CMD:[%s] ARG[1]=%s", __FUNCTION__,
            pcCommand, (argv[1] == NULL) ? "NULL" : argv[1]);

        if (pcSearchStr != NULL)
        {
            close(pfd[0]);
            if (dup2(pfd[1], STDOUT_FILENO) < 0)
            {
                ERR("%s Failed to duplicating STDOUT error: %s",
                    __FUNCTION__, strerror(errno));
                _exit(EXIT_FAILURE);
            }
        }

        /* Change the priority of the process to very low */
        param.sched_priority = sched_get_priority_min(SCHED_OTHER);
        if (sched_setscheduler(0, SCHED_OTHER, &param) < 0)
        {
            ERR("%s Failed to set scheduling priority error: %s",
                __FUNCTION__, strerror(errno));
            _exit(EXIT_FAILURE);
        }

        if (execv(pcCommand, argv) < 0)
        {
            ERR("%s Failed to execute CMD:[%s] error: %s",
                __FUNCTION__, pcCommand, strerror(errno));
            _exit(EXIT_FAILURE);
        }
    }


    /* Parent Process */
    else
    {
        fd_set          readfds;
        struct timeval  tm;
        char            cLine[LINE_MAX];
        int             status  = 0;
        int             result  = 0;

        if (pcSearchStr != NULL)
        {
            FD_ZERO(&readfds);
            FD_SET(pfd[0], &readfds);
            close(pfd[1]);
        }
        memset(cLine, 0, sizeof(cLine));

        tm.tv_sec  = lTimeoutMs / 1000;
        tm.tv_usec = (lTimeoutMs - (tm.tv_sec * 1000)) * 1000;

        M_("%s, Parent: Waiting for CMD", __FUNCTION__);

        /* Save the PID so we can send the TERM later */
        if (pChildPid != NULL)
        {
            *pChildPid = pid;
        }

        /* Now lets makes sure the process is gone, if not then kill it */
        if (lTimeoutMs == -1)
        {
            while ((pStopCondition == NULL) ? 1 : !(*pStopCondition))
            {
                result = waitpid(pid, &status, WNOHANG | WUNTRACED);
                if (result != 0)
                {
                    break;
                }
                usleep(10);
            }
            if ( (pStopCondition != NULL) && (*pStopCondition) )
            {
                M_("%s, Received stop condition, canceling...", __FUNCTION__);
                tRetCode = -1;
            }
        }
        else
        {
            for (i = 0; i < lTimeoutMs; i+=10)
            {
                result = waitpid(pid, &status, WNOHANG | WUNTRACED);
                if (result != 0)
                {
                    break;
                }
                usleep(10);
            }
            if (result == 0)
            {
                M_("%s, Parent:[%s] Timeout", __FUNCTION__, pcCommand);
                tRetCode = -1;
            }
        }

        if (result < 0 )
        {
            ERR("%s waitpid failed! error: %s", __FUNCTION__, strerror(errno));
        }

        /* Return value of 0 means that it is still running and we need to kill it */
        else if (result == 0)
        {
            M_("%s, Parent: Need to send kill to child", __FUNCTION__);
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
        }

        /* Terminated, lets see the return code */
        else if ( WIFEXITED(status) && (WEXITSTATUS(status) == 0) )
        {
        	M_("%s, Parent: Command Finished gracefully", __FUNCTION__);

            /* Parse Results */
            if (pcSearchStr != NULL)
            {
                FILE *fd = fdopen(pfd[0], "r");
                if (fd != NULL)
                {
                    while (fgets(cLine, sizeof(cLine), fd) != NULL)
                    {
                        char *pcResult = NULL;

                        M_("%s", cLine);
                        if ((pcResult = strstr(cLine, pcSearchStr)) != NULL)
                        {
                            pcResult += strlen(pcSearchStr);
                            M_("%s, Parent: CMD[%s] Success!", __FUNCTION__, pcCommand);
                            if ( (pResults != NULL) && ulLength)
                            {
                                if (pcResult != NULL)
                                {
                                    strncpy(pResults, pcResult, ulLength);
                                }
                                else
                                {
                                    memset(pResults, 0, ulLength);
                                }
                            }
                            tRetCode = 0;
                            break;
                        }
                    }
                    fclose(fd);
                }
                if (tRetCode != 0)
                {
                    tRetCode = -1;
                }
                close(pfd[0]);
            }
            else
            {
                M_("%s, Parent: CMD[%s] Success!", __FUNCTION__, pcCommand);
                tRetCode = 0;
            }
        }
    }

err_out:
    if (pBuf != NULL)
    {
        free(pBuf);
    }
    M_("%s(): Leaving, Result=%d", __FUNCTION__, tRetCode);
    return tRetCode;
}

int xwifi_sys_start_udhcpc(const char* ifname)
{
	char cmd[512];
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "/sbin/udhcpc -i %s -p /tmp/udhcpc.%s.pid", ifname, ifname);
	sleep(1);
	return system(cmd);
//	return xwifi_sys_system("/sbin/udhcpc", args, NULL, -1, NULL, NULL, 0, NULL);
}

int xwifi_sys_stop_udhcpc(const char* ifname)
{
	char cmd[512];
	memset(cmd, 0, sizeof(cmd));

	char file[128];
	snprintf(file, sizeof(file), "/tmp/udhcpc.%s.pid", ifname);

    FILE *fp = fopen(file, "r");
    char cLine[24];
    if (fp != NULL) {
        if (fgets(cLine, sizeof(cLine), fp) != NULL) {
            int32_t pid = atoi(cLine);
//            kill(pid, fp);
            kill(pid, SIGTERM);
        }
        fclose(fp);
    }
	sleep(1);
	return 0;
}

int xwifi_sys_restart_udhcpc(const char* ifname)
{
	S_;
	xwifi_sys_stop_udhcpc(ifname);
	return xwifi_sys_start_udhcpc(ifname);
}

int xwifi_sys_ifconfig(const char* ifname, bool up)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq    ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
	int err = ioctl(sock, SIOCGIFFLAGS, &ifr);
	KASSERT(err==0);

	ifr.ifr_flags = (up) ? (ifr.ifr_flags | IFF_UP | IFF_RUNNING) : (ifr.ifr_flags & ~(IFF_UP | IFF_RUNNING));
	err = ioctl(sock, SIOCSIFFLAGS, &ifr);
	KASSERT(err==0);
	close(sock);
	return err;
}

int xwifi_sys_ifconfig_clear(const char* ifname)
{
	KASSERT(ifname);
	struct ifreq        ifr;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in  *p = (struct sockaddr_in *)&(ifr.ifr_addr);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
	p->sin_family = AF_INET;
	int err = ioctl(sock, SIOCGIFADDR, &ifr);
	if((err >= 0) && (p->sin_addr.s_addr != 0)){
	    /* Set IP Address to 0 to kill all routes, this might fail
	     * if there are no routes so we ignore the error */
	    p->sin_addr.s_addr = 0;
	    err = ioctl(sock, SIOCSIFADDR, &ifr);
	    if (err < 0) {

	    }
	}
	close(sock);
	return err;
}

unsigned int xwifi_sys_get_ip_address(const char* ifname)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	unsigned int addr;
	struct ifreq        ifr;
	struct sockaddr_in  *p = (struct sockaddr_in *)&(ifr.ifr_addr);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
	p->sin_family = AF_INET;

	int err = ioctl(sock, SIOCGIFADDR, &ifr);
	close(sock);

	if(err >= 0) {
		return p->sin_addr.s_addr;
	}
	return -1;
}

