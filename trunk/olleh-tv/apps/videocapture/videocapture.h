/*
 * videocapture.h
 *
 *  Created on: Feb 7, 2013
 *      Author: buttonfly
 */

#ifndef VIDEOCAPTURE_H_
#define VIDEOCAPTURE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>

#define CAP_FORMAT_MJPEG_PRIV	"mjpeg"
#define CAP_FORMAT_H264_PRIV		"h264"
#define CAP_FORMAT_YUYV_PRIV		"yuyv"
typedef void* videocapture;

struct videocapture_frame {
    unsigned char          *data;               /* Frame data */
    int               size;               /* The size of frame data */
    int               index;              /* The index of frame */
    struct timeval	  timeStamp;          /* Time Stamp */
};

videocapture videocapture_open(const char* dev, const char* fmt, int width, int height, int framerate);

int videocapture_start(videocapture h);

int videocapture_capture(videocapture h, struct videocapture_frame* frame);

int videocapture_close(videocapture h);


#ifdef __cplusplus
}
#endif


#endif /* VIDEOCAPTURE_H_ */
