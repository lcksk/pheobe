/*
 * xwifi_event.c
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

#include <pthread.h>
#include "nds/xwifi.h"
#include "xwifi_dbg.h"
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>


#define UNIX_DOMAIN_FILE_PATH	"/tmp/.un"

struct xwifi_event_queue {
	int fd;
	uint32_t events;
	pthread_mutex_t lck;
	fd_set readfds;
	struct sockaddr_un  serveraddr;
};

/* internal use functions */
static struct xwifi_event_queue* xwifi_event_queue_get_instance();
static int xwifi_event_queue_server_initialize(struct xwifi_event_queue* event_queue, const char* file);

int xwifi_open_event_queue(_out_ int* const fd)
{
	S_;
	struct xwifi_event_queue* event_queue =xwifi_event_queue_get_instance();
	KASSERT(event_queue);
	*fd = event_queue->fd;
	return *fd;
}

int xwifi_close_event_queue(const int fd)
{
	S_;
	struct xwifi_event_queue* event_queue = xwifi_event_queue_get_instance();
	unlink(UNIX_DOMAIN_FILE_PATH);
	close(event_queue->fd);
	free(event_queue);
	return 0;
}

int32_t xwifi_subscribe_events(const int fd, const uint32_t events)
{
	S_;
	struct xwifi_event_queue* event_queue =xwifi_event_queue_get_instance();
	event_queue->events = events;
	return 0;
}

int32_t xwifi_get_event(const int fd, _out_ XWifiEvent* const event)
{
	S_;
	int nread, offset;
	int clientsocket = -1;
	const int sizeof_event = sizeof(*event);
	struct sockaddr_un clientaddr;
	int clientaddr_len;
	clientaddr_len = sizeof(clientaddr);

	struct xwifi_event_queue* self =xwifi_event_queue_get_instance();
	pthread_mutex_lock(&self->lck);
	clientsocket = accept(fd, (struct sockaddr_un*) &clientaddr, (socklen_t*) &clientaddr_len);

	KASSERT(clientsocket > 0);
	if(clientsocket < 0) {
		ERR("invalid client socket: %d", clientsocket);
		pthread_mutex_unlock(&self->lck);
		return -1;
	}

	ioctl(clientsocket, FIONREAD, &nread);
	M_("nread: %d", nread);
	if(nread == 0) {
		ERR("nread: %d", nread);
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

int32_t xwifi_put_event(const int _fd, XWifiEvent* const event)
{
	S_;
	int fd;
	int ret;
	struct sockaddr_un serveraddr;
	int nwrite, offset;
	const int sizeof_event = sizeof(*event);

	struct xwifi_event_queue* event_queue =xwifi_event_queue_get_instance();
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

	M_("size : %d", sizeof_event);
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

static struct xwifi_event_queue* xwifi_event_queue_get_instance()
{
	static struct xwifi_event_queue* event_queue = NULL;
	if(event_queue == NULL) {
		event_queue = malloc(sizeof(*event_queue));
		pthread_mutex_init(&event_queue->lck, NULL);
		xwifi_event_queue_server_initialize(event_queue, UNIX_DOMAIN_FILE_PATH);
	}
	return event_queue;
}

static int xwifi_event_queue_server_initialize(struct xwifi_event_queue* self, const char* file)
{
	S_;
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




