/*
 * main.c
 *
 *  Created on: Apr 8, 2013
 *      Author: buttonfly
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <assert.h>
#include "multicastcapture.h"
#include <uriparser/Uri.h>


typedef enum {
	eUDP = 0,
	eRTP
} eProtocol;

typedef struct  {
	char* multicast_ip;
	unsigned short multicast_port;
	char* filepath;
	FILE* fp;
	eProtocol prot;
} application_info_t;

static application_info_t* getcontext() {
	static application_info_t context;
	return &context;
}


static void usage(const char* program);
static void decode_options(application_info_t* application_info, int argc, char **argv);

size_t data_received(void* context, u_int8_t* buf, size_t len) {
	return fwrite(buf, 1, len, (FILE*) context);
}

int main(int argc, char** argv)
{
	application_info_t* context = getcontext();
	memset(context, 0, sizeof(application_info_t));
	decode_options(context, argc, argv);

	if(context->multicast_ip == NULL || context->multicast_port < 0) {
		usage(argv[0]);
		exit(0);
	}

	if(context->filepath) {
		context->fp  = fopen(context->filepath, "wt+");
	}
	else {
		context->fp  = stdout;
	}

	multicastcapture_open_param_t param;
	memset(&param, 0, sizeof(multicastcapture_open_param_t));
	param.ip = (int8_t*)context->multicast_ip;
	param.port = context->multicast_port;
	param.cbr.cbr = data_received;
	param.cbr.context = context->fp;
	multicastcapture cap = multicastcapture_open(&param);
	multicastcapture_start(cap);
	multicastcapture_stop(cap);
	multicastcapture_close(cap);

	return 0;
}

static void usage(const char* program){

	printf("Usage: %s <options>\n", program);
	printf("Options:\n"
			"\t-h, -help\n"
			"\t\tPrint this help\n"
			"\t-i, --ip <ip>\n"
			"\t\tmulticast ip address\n"
			"\t-p, --port\n"
			"\t\tmulticast port\n"
			"\t-d, --dst\n"
			"\t\tfile to store\n"
			"\tex: %s -i 233.13.231.231 -p 5000\n", program
	);
}

static void decode_options(application_info_t* application_info, int argc, char **argv)
{
    static const char *opt_string="i:p:d:h";
    static struct option const longopts[]   =  {
        {"ip",required_argument,NULL,'i'},
        {"port",required_argument,NULL,'p'},
        {"dst",required_argument,NULL,'d'},
        {"help",no_argument,NULL,'h'},
        {NULL,0,NULL,0}
    };
    int optc, longind=0;
    const char *name = argv[0];

    while((optc=getopt_long(argc,argv,opt_string,longopts,&longind))!=-1)
    {
        switch (optc)
        {
        case 'h':
        	usage(name);
        	exit(0);

        case 'i':
        	application_info->multicast_ip = strdup(optarg);
        	break;

        case 'p':
        	application_info->multicast_port = atoi(optarg);
        	break;

        case 'd':
        	application_info->filepath = strdup(optarg);
        	break;

        default:
        	usage(name);
        	exit(0);
        }
    }

    int i;
    for(i = optind; i < argc; i++) {
//    	application_info->uri = strdup(argv[i]);
    	fprintf(stderr, "argv[%d]: %s\n", i, argv[i]);

    	UriParserStateA state;
        UriUriA _uri;
    	state.uri = &_uri;
    	char* uri = argv[i];
        if (uriParseUriA(&state, uri) != URI_SUCCESS) {
                /* Failure */
    		uriFreeUriMembersA(&_uri);
    		fprintf(stderr, "failed to <uriParseUriA>\n");
    		exit(0);
        }

        if(memcmp("rtp", _uri.scheme.first, _uri.scheme.afterLast - _uri.scheme.first)==0) {
        	application_info->prot = eRTP;
        }
        else if(memcmp("udp", _uri.scheme.first, _uri.scheme.afterLast - _uri.scheme.first)==0) {
        	application_info->prot = eUDP;
        }
        else {
        	fprintf(stderr, "Unknown protocol\n");
        	usage(name);
        }

        /* group address */
        char tmp[512] = {0};
        sprintf(tmp, "%d.%d.%d.%d", _uri.hostData.ip4->data[0], _uri.hostData.ip4->data[1], _uri.hostData.ip4->data[2], _uri.hostData.ip4->data[3]);
        if(application_info->multicast_ip)
        	free(application_info->multicast_ip);
        application_info->multicast_ip = strdup(tmp);

        /* port */
        application_info->multicast_port = atoi(_uri.portText.first);
        uriFreeUriMembersA(&_uri);
    }
}

