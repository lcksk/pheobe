/*
 * videocapture.c
 *
 *  Created on: Feb 7, 2013
 *      Author: buttonfly
 */

#include "videocapture.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/videodev2.h>
#include "libv4l2.h"
#include <linux/input.h>
#include <linux/soundcard.h>

struct video_buffer {
    void *            start;
    size_t            len;
};


struct videocapture_t
{
	struct {
		int fd;
		unsigned int n_buff;
		struct video_buffer* buff;
		struct v4l2_format fmt;
	} video;

	struct {
		int fd;
	} audio;
};

static void __ioctl(int fd, int request, void *arg)
{
	int r;

	do {
		r = v4l2_ioctl(fd, request, arg);
	} while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

	if (r == -1) {
		fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}


videocapture videocapture_open(const char* dev,  const char* fmt,  int width, int height, int framerate)
{
	struct v4l2_requestbuffers      req;
	struct v4l2_buffer              buf;
	unsigned int                    i,n_buffers;
	enum v4l2_buf_type              type;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;

	struct videocapture_t* obj = (struct videocapture_t*) malloc(sizeof(struct videocapture_t));
	memset(obj, 0, sizeof(struct videocapture_t));

	obj->video.fd = v4l2_open(dev, O_RDWR | O_NONBLOCK, 0);

	{
		struct v4l2_streamparm parm;
		memset(&parm, 0, sizeof parm);
		parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		  __ioctl(obj->video.fd, VIDIOC_G_PARM, &parm);
		fprintf(stderr, "Current frame rate: %u/%u\n",
			parm.parm.capture.timeperframe.numerator,
			parm.parm.capture.timeperframe.denominator);

		parm.parm.capture.timeperframe.numerator = 1;
		parm.parm.capture.timeperframe.denominator = framerate;
		  __ioctl(obj->video.fd, VIDIOC_S_PARM, &parm);

		parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		  __ioctl(obj->video.fd, VIDIOC_G_PARM, &parm);
		fprintf(stderr, "Current frame rate: %u/%u\n",
			parm.parm.capture.timeperframe.numerator,
			parm.parm.capture.timeperframe.denominator);
	}


	if(strcmp(fmt, CAP_FORMAT_H264_PRIV)==0) {
		memset(&obj->video.fmt, 0, sizeof(struct v4l2_format));
		obj->video.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		obj->video.fmt.fmt.pix.width       = width;
		obj->video.fmt.fmt.pix.height      = height;
		obj->video.fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
		obj->video.fmt.fmt.pix.field       = V4L2_FIELD_ANY;
	    __ioctl(obj->video.fd, VIDIOC_S_FMT, &obj->video.fmt);
	    if (obj->video.fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_H264) {
	            printf("Libv4l didn't accept RGB24 format. Can't proceed.\n");
	            exit(EXIT_FAILURE);
	    }
	}
	else if(strcmp(fmt, CAP_FORMAT_MJPEG_PRIV)==0) {
		memset(&obj->video.fmt, 0, sizeof(struct v4l2_format));
		obj->video.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		obj->video.fmt.fmt.pix.width       = width;
		obj->video.fmt.fmt.pix.height      = height;
		obj->video.fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
		obj->video.fmt.fmt.pix.field       = V4L2_FIELD_ANY;
	    __ioctl(obj->video.fd, VIDIOC_S_FMT, &obj->video.fmt);
	    if (obj->video.fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG) {
	            printf("Libv4l didn't accept RGB24 format. Can't proceed.\n");
	            exit(EXIT_FAILURE);
	    }
	}
	else if(strcmp(fmt, CAP_FORMAT_YUYV_PRIV)==0) {
		memset(&obj->video.fmt, 0, sizeof(struct v4l2_format));
		obj->video.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		obj->video.fmt.fmt.pix.width       = width;
		obj->video.fmt.fmt.pix.height      = height;
		obj->video.fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		obj->video.fmt.fmt.pix.field       = V4L2_FIELD_ANY;
	    __ioctl(obj->video.fd, VIDIOC_S_FMT, &obj->video.fmt);
	    if (obj->video.fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
	            printf("Libv4l didn't accept RGB24 format. Can't proceed.\n");
	            exit(EXIT_FAILURE);
	    }
	}
	else {
		fprintf(stderr, "This example doesn't support this format: %s\n", fmt);
		exit(EXIT_FAILURE);
	}

    if ((obj->video.fmt.fmt.pix.width != width) || (obj->video.fmt.fmt.pix.height != height))
            printf("Warning: driver is sending image at %dx%d\n",
            		obj->video.fmt.fmt.pix.width, obj->video.fmt.fmt.pix.height);

    memset(&req, 0, sizeof(struct v4l2_requestbuffers));
    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    __ioctl(obj->video.fd, VIDIOC_REQBUFS, &req);

    obj->video.buff = (struct video_buffer*) calloc(req.count, sizeof(struct video_buffer ));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
            memset(&buf, 0, sizeof(struct v4l2_buffer));

            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory      = V4L2_MEMORY_MMAP;
            buf.index       = n_buffers;

            __ioctl(obj->video.fd, VIDIOC_QUERYBUF, &buf);

            obj->video.buff[n_buffers].len = buf.length;
            obj->video.buff[n_buffers].start = v4l2_mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, obj->video.fd, buf.m.offset);

            if (MAP_FAILED ==  obj->video.buff[n_buffers].start) {
                    perror("mmap");
                    exit(EXIT_FAILURE);
            }
    }

    for (i = 0; i < n_buffers; ++i) {
            memset(&buf, 0, sizeof(struct v4l2_buffer));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            __ioctl(obj->video.fd, VIDIOC_QBUF, &buf);
    }

	return obj;
}

int videocapture_start(videocapture h)
{
	struct videocapture_t* obj = (struct videocapture_t*) h;
	enum v4l2_buf_type              type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    __ioctl(obj->video.fd,  VIDIOC_STREAMON, &type);

	return 0;
}

int videocapture_capture(videocapture h, struct videocapture_frame* frame)
{
	struct videocapture_t* obj = (struct videocapture_t*) h;
	fd_set                          fds;
	int                             r;
	struct timeval                  tv;
	struct v4l2_buffer              buf;

    do {
            FD_ZERO(&fds);
            FD_SET(obj->video.fd, &fds);
            /* Timeout. */
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            r = select(obj->video.fd + 1, &fds, NULL, NULL, &tv);

    } while ((r == -1 && (errno = EINTR)));
    if (r == -1) {
            perror("select");
            return errno;
    }

    memset(&buf, 0, sizeof(struct v4l2_buffer));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    __ioctl(obj->video.fd, VIDIOC_DQBUF, &buf);

#if 0
	char                            out_name[256];
	FILE                            *fout;

	static int i = 0;
	i++;
    sprintf(out_name, "out%03d.ppm", i);
    fout = fopen(out_name, "w");
    if (!fout) {
            perror("Cannot open image");
            exit(EXIT_FAILURE);
    }
    fprintf(fout, "P6\n%d %d 255\n", obj->video.fmt.fmt.pix.width, obj->video.fmt.fmt.pix.height);
    fwrite(obj->video.buff[buf.index].start, buf.bytesused, 1, fout);
    fclose(fout);
#endif

    frame->data= (unsigned char*)obj->video.buff[buf.index].start;
    frame->size = buf.bytesused;
    frame->index = buf.index;
    frame->timeStamp = buf.timestamp;
    __ioctl(obj->video.fd, VIDIOC_QBUF, &buf);
    return 0;
}

int videocapture_close(videocapture h)
{
	struct videocapture_t* obj = (struct videocapture_t*) h;
	enum v4l2_buf_type              type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    __ioctl(obj->video.fd, VIDIOC_STREAMOFF, &type);
    int i;
    int n_buffers = 2;
    for (i = 0; i < n_buffers; ++i)
            v4l2_munmap(obj->video.buff[i].start, obj->video.buff[i].len);
    v4l2_close(obj->video.fd);

	free(obj);
	return 0;
}


