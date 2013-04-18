/*
 * main.c
 *
 *  Created on: Feb 7, 2013
 *      Author: buttonfly
 *
 *      Simple example.
 *
 *      How to verify the output file (raw video):
 *		 ex) ffmpeg -f h264 -i capture.ts -vcodec copy test.mp4
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <signal.h>


#include "videocapture.h"

#define VIDEO_CAMERA_WIDTH	160
#define VIDEO_CAMERA_HEIGHT	120

#define DEFAULT_VIDEO_DEV	"/dev/video0"
#define DEFAULT_OUTPUT			"capture.raw"
#define DEFAULT_FRAMERATE	30


typedef struct {
	char* interface;
	char* output;
	char* format;
	int framerate;
	int width;
	int height;
	videocapture cap;
} application_info_t;

static void decode_options(int argc, char **argv, application_info_t* application_info);
static void usage(const char* program);
static void signalhandler(int signo);
static application_info_t* getcontext();

int main(int argc, char** argv)
{
	application_info_t* info = getcontext();
	memset(info, 0, sizeof(application_info_t));

	signal(SIGINT, signalhandler);
	signal(SIGTERM, signalhandler);

	decode_options(argc, argv, info);

	// set defaults
	if(!info->interface) {
		info->interface = strdup(DEFAULT_VIDEO_DEV);
	}

#if 0
	if(!info->output) {
		info->output = strdup(DEFAULT_OUTPUT);
	}
#endif

	if(!info->format) {
		info->format = strdup(CAP_FORMAT_H264_PRIV);
	}

	if(info->framerate == '\0') {
		info->framerate = DEFAULT_FRAMERATE;
	}

	if(info->width == '\0') {
		info->width = VIDEO_CAMERA_WIDTH;
	}

	if(info->height == '\0') {
		info->height = VIDEO_CAMERA_HEIGHT;
	}

	fprintf(stderr, "%s\n", info->interface);
	fprintf(stderr, "%s\n", info->output);
	fprintf(stderr, "%s\n", info->format);

    videocapture cap = videocapture_open(info->interface, info->format, info->width, info->height, info->framerate);
    info->cap = cap;
    videocapture_start(cap);

    FILE* fp = NULL;
    if(info->output == NULL) {
    	fp = stdout;
    }
    else {
    	fp = fopen(info->output, "wt+");
    }

    if(!fp) {
    	fprintf(stderr, "failed to open %s\n", info->output);
    	exit(EXIT_FAILURE);
    }

    for(;;) {

		struct videocapture_frame frame;
		memset(&frame, 0, sizeof(struct videocapture_frame));
		videocapture_capture(cap, &frame);

#if 0
		fwrite(frame.data, frame.size, 1, fp);
#else
		fwrite(frame.data, 1, frame.size,  fp);
#endif
    }

    videocapture_close(cap);
    fclose(fp);
    return 0;
}

static void decode_options(int argc, char **argv, application_info_t* application_info)
{
    static const char *opt_string="i:o:f:r:s:h";
    static struct option const longopts[]   =  {
        {"if",required_argument,NULL,'i'},
        {"output",required_argument,NULL,'o'},
        {"format",required_argument,NULL,'f'},
        {"rate",required_argument,NULL,'r'},
        {"size",required_argument,NULL,'s'},
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

        case 'i':
        	application_info->interface = strdup(optarg);
        	break;

        case 'o':
        	application_info->output = strdup(optarg);
        	break;

        case 'f':
        	application_info->format = strdup(optarg);
        	break;

        case 'r':
        	application_info->framerate = atoi(optarg);
        	break;

        case 's':
        	sscanf(optarg, "%dx%d", &application_info->width, &application_info->height);
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
			"\t-i, --if <device>\n"
			"\t\tdevice (default: /dev/video0)\n",
			"\t-o, --output <capture file name>\n"
			"\t\tcapture file name (default: capture.raw)\n"
			"\t-f, --format <h264 | mjpeg>\n"
			"\t\traw video format (default: h264)\n"
			"\t-f, --size <widthxheight>\n"
			"\t\tsize (default: 160x120)\n"
			,program
	);
}

static void signalhandler(int signo) {
	sigset_t    sigset, oldset;
	sigfillset(&sigset);
	sigprocmask(SIG_BLOCK, &sigset, &oldset);

    switch(signo) {
    case SIGTERM:
    case SIGINT:
    case SIGSEGV:
    	videocapture_close(getcontext()->cap);
    	exit(1);
    	break;
    }
    sigprocmask(SIG_SETMASK, &oldset, NULL);
}


static application_info_t* getcontext() {
	static application_info_t context;
	return &context;
};
