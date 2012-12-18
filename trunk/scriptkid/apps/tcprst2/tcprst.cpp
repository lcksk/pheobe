/*
 * tcprst.c
 *
 *  Created on: Dec 14, 2012
 *      Author: buttonfly
 */


#include <stdio.h>
#include <string.h>
#include <libnet.h>
#include <getopt.h>
#include "dbg.h"
#include "CFile.h"
#include "CGKeyFile.h"

#define PATH_MAX 512
#define CONF_FILE_NAME	"env.conf"

struct addr {
	unsigned long ip;
	unsigned short port;
};

typedef struct {
	char conf_path[PATH_MAX];
	struct addr src;
	struct addr dst;
	unsigned short ttl;
	unsigned long interval;
} application_info_t;


static void decode_options(application_info_t* application_info, int argc, char **argv);
static void usage(const char* program);
static int validate_conf(libnet_t* handle, const char* conf, application_info_t* application_info);
static int perform(libnet_t* handle, application_info_t* application_info);

int main(int argc, char** argv) {

	int uid = getuid();
	if(uid != 0) {
		M_("permission denied!");
		return 0;
	}

	static application_info_t application_info;

	decode_options(&application_info, argc, argv);
	if(strlen(application_info.conf_path) == 0) {
		printf("default conf: <%s>\n", CONF_FILE_NAME);
		strcpy(application_info.conf_path, CONF_FILE_NAME);
	}

	char err[LIBNET_ERRBUF_SIZE];
	libnet_t* handle = libnet_init(LIBNET_RAW4, NULL, err);
	KASSERT(handle);

	validate_conf(handle, application_info.conf_path, &application_info);
	perform(handle, &application_info);

	libnet_destroy(handle);
	return 0;
}

static void decode_options(application_info_t* application_info, int argc, char **argv)
{
    static const char *opt_string="c:h";
    static struct option const longopts[]   =  {
        {"config",optional_argument,NULL,'c'},
        {"help",no_argument,NULL,'h'},
        {NULL,0,NULL,0}
    };
    int optc, longind=0;
    const char *name = argv[0];

    while((optc=getopt_long(argc,argv,opt_string,longopts,&longind))!=-1) {

        switch (optc) {
        case 'h':
        	usage(name);
        	exit(0);

        case 'c':
        	strcpy(application_info->conf_path, optarg);
        	break;

        default:
        	usage(name);
        	exit(0);
        }
    }
}


static void usage(const char* program) {
	printf("Usage: %s <options>\n", program);
	printf("Options:\n"
			"\t-h, -help\n"
			"\t\tPrint this help\n"
			"\t-c, --config <path>\n"
			"\t\tconf file path (default: env.conf)\n"
	);
}

static int validate_conf(libnet_t* handle, const char* conf, application_info_t* application_info) {

	CFile confFile(conf);
	if(confFile.Exist() == false) {
		fprintf(stderr, "conf file: <%s> doesn't exist\n", conf);
		fprintf(stderr, "sample conf. file's been created. modify appropriately\n");
		CGKeyFile configfile(CONF_FILE_NAME);
		configfile.Set("src", "ip", "14.77.129.218");
		configfile.Set("src", "port", "7080");

		configfile.Set("dst", "ip", "175.214.47.128");
		configfile.Set("dst", "port", "8150");

		configfile.Set("etc", "ttl", 64);
		configfile.Set("etc", "interval", 100);
		configfile.Store();
		configfile.Close();
	}

	CGKeyFile configfile(conf);
	configfile.Load();

	application_info->src.ip = libnet_name2addr4(handle, (char*)configfile.Get("src", "ip"), LIBNET_RESOLVE);
	application_info->src.port = configfile.GetAsInt("src", "port");

	application_info->dst.ip = libnet_name2addr4(handle, (char*)configfile.Get("dst", "ip"), LIBNET_RESOLVE);
	application_info->dst.port = configfile.GetAsInt("dst", "port");

	application_info->ttl = configfile.GetAsInt("etc","ttl");
	application_info->interval = configfile.GetAsInt("etc","interval");
	return 0;
}

static int perform(libnet_t* handle, application_info_t* application_info) {

	libnet_ptag_t t;
#if 0
	t = libnet_build_tcp_options(
	        (uint8_t*)"\003\003\012\001\002\004\001\011\010\012\077\077\077\077\000\000\000\000\000\000",
	        20,
	        handle,
	        0);
#endif

    t = libnet_build_tcp(
        application_info->src.port,                                    /* source port */
        application_info->dst.port,                                    /* destination port */
        1,                                 /* sequence number */
        0,                                 /* acknowledgement num */
        TH_RST,                                     /* control flags */
        0,                                      /* window size */
        0,                                          /* checksum */
        10,                                          /* urgent pointer */
        LIBNET_TCP_H + 20 + 0,              /* TCP packet size */
        (uint8_t*) NULL,                         /* payload */
        0,                                  /* payload size */
        handle,                                          /* libnet handle */
        0);                                         /* libnet id */

    KASSERT(t != -1);

    t = libnet_build_ipv4(
        LIBNET_IPV4_H + LIBNET_TCP_H + 20 + 0,/* length */
      	0,                                          /* TOS */
        242,                                        /* IP ID */
        0,                                          /* IP Frag */
        application_info->ttl,                                         /* TTL */
        IPPROTO_TCP,                                /* protocol */
        0,                                          /* checksum */
        application_info->src.ip,                                     /* source IP */
        application_info->dst.ip,                                     /* destination IP */
        NULL,                                       /* payload */
        0,                                          /* payload size */
        handle,                                          /* libnet handle */
        0);                                         /* libnet id */

    KASSERT(t != -1);

#if 0
    t = libnet_build_ethernet(
        enet_dst,                                   /* ethernet destination */
        enet_src,                                   /* ethernet source */
        ETHERTYPE_IP,                               /* protocol type */
        NULL,                                       /* payload */
        0,                                          /* payload size */
        handle,                                          /* libnet handle */
        0);                                         /* libnet id */
#endif


    KASSERT(t != -1);
    for(;;) {
    	int c = libnet_write(handle);
    	if(c == -1) {
    		printf("failed to send a packet: %d", c);
    	}

        if(application_info->interval> 0) {
        	usleep(application_info->interval);
        }
    }


}
