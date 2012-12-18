
/*
 * osl_event.c
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */

/**
 * @description:
 * We assume that a event-generator and a event-receiver runs on the same process.
 * However, we go with UNIX DOMAIN SOCKET because the client software use 'poll' API to detect an event.
 * UNIX_DOMAIN_SOCKET meets all needs in this case.
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "osl.h"

#define UNIX_DOMAIN_FILE_PATH	"/tmp/.un"

struct osl_event_queue {
	int fd;
	uint32_t events;
	pthread_mutex_t lck;
	fd_set readfds;
	struct sockaddr_un  serveraddr;
};

/* internal use functions */
static struct osl_event_queue* oslEventQueGetInstance();
static int oslEventQueInitialise(struct osl_event_queue* event_queue, const char* file);

int oslEventQueOpen(int* const fd)
{
	struct osl_event_queue* event_queue =oslEventQueGetInstance();
	if(fd) {
		*fd = event_queue->fd;
	}
	return event_queue->fd;
}

int oslEventQueClose(const int fd)
{
	struct osl_event_queue* event_queue = oslEventQueGetInstance();
	unlink(UNIX_DOMAIN_FILE_PATH);
	close(event_queue->fd);
	free(event_queue);
	return 0;
}

int32_t oslEventQueSubscribe(const int fd, const uint32_t events)
{
	struct osl_event_queue* event_queue =oslEventQueGetInstance();
	event_queue->events = events;
	return 0;
}

int32_t oslEventQueGet(const int fd, osl_event* const event)
{
	int nread, offset;
	int clientsocket = -1;
	const int sizeof_event = sizeof(*event);
	struct sockaddr_un clientaddr;
	int clientaddr_len;
	clientaddr_len = sizeof(clientaddr);

	struct osl_event_queue* self =oslEventQueGetInstance();
	pthread_mutex_lock(&self->lck);
	clientsocket = accept(fd, (struct sockaddr_un*) &clientaddr, (socklen_t*) &clientaddr_len);

	if(clientsocket < 0) {
		fprintf(stderr, "invalid client socket: %d", clientsocket);
		pthread_mutex_unlock(&self->lck);
		return -1;
	}

	ioctl(clientsocket, FIONREAD, &nread);
	if(nread == 0) {
		fprintf(stderr, "nread: %d", nread);
		close(clientsocket);
		pthread_mutex_unlock(&self->lck);
		return -1;
	}

	for(nread=0, offset=0; offset < sizeof_event; offset+=nread) {
		nread = read(clientsocket, event + offset, sizeof_event - offset);
		usleep(10);
		fprintf(stderr, ".");
	}
	pthread_mutex_unlock(&self->lck);
	return 0;
}

int32_t oslEventQuePut(const int _fd, osl_event* const event)
{
	int fd;
	int ret;
	struct sockaddr_un serveraddr;
	int nwrite, offset;
	const int sizeof_event = sizeof(*event);

	struct osl_event_queue* event_queue =oslEventQueGetInstance();
	if(!(event->event & event_queue->events)) {
		fprintf(stderr, "event <%d> filtered", event->event);
		return -1;
	}

	pthread_mutex_lock(&event_queue->lck);
	fd = socket(PF_UNIX, SOCK_STREAM, 0);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, UNIX_DOMAIN_FILE_PATH);

    ret = connect(fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	if(ret == -1) {
		fprintf(stderr, "oops! cannot connect to ");
		pthread_mutex_unlock(&event_queue->lck);
		return -1;
	}

	for(nwrite=0, offset=0; offset < sizeof_event; offset+=nwrite) {
		nwrite = write(fd, event + offset, sizeof_event - offset);
		usleep(10);
		fprintf(stderr, ",");
	}

	close(fd);
	pthread_mutex_unlock(&event_queue->lck);
	usleep(100); // yield
	return 0;
}

static struct osl_event_queue* oslEventQueGetInstance()
{
	static struct osl_event_queue* event_queue = NULL;
	if(event_queue == NULL) {
		event_queue = malloc(sizeof(*event_queue));
		pthread_mutex_init(&event_queue->lck, NULL);
		oslEventQueInitialise(event_queue, UNIX_DOMAIN_FILE_PATH);
	}
	return event_queue;
}

static int oslEventQueInitialise(struct osl_event_queue* self, const char* file)
{
	if (access(file, R_OK | W_OK) == 0) {
		unlink(file);
	}

	self->fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (self->fd  < 0) {
		fprintf(stderr, "failed to create a socket descriptor");
		return -1;
	}

	memset(&self->serveraddr, 0, sizeof(self->serveraddr));
	self->serveraddr.sun_family = AF_UNIX;
	strcpy(self->serveraddr.sun_path, file);

	bind(self->fd, (struct sockaddr*) &self->serveraddr, sizeof(self->serveraddr));
	listen(self->fd, 5);
	return 0;
}



