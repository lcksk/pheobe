/*
 * PlaybackService.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#include <stdio.h>
#include <string.h>
#include "PlaybackService.h"
#include "GstPlaybin.h"
#include "stringtokenizer.h"

const char* halkamalka::PlaybackService::ID = "PlaybackService";

namespace halkamalka {

PlaybackService::PlaybackService() {
    pthread_attr_t attri;
    pthread_attr_init(&attri);
    pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_lck, NULL);
    pthread_cond_init(&m_cond, NULL);

    pthread_attr_destroy(&attri);
}

PlaybackService::~PlaybackService() {
	// TODO Auto-generated destructor stub
}


const char* PlaybackService::GetID()
{
	return ID;
}


void PlaybackService::Run() {

	for(;;) {
		if(m_multicastcap) {
			multicastcapture_start(m_multicastcap);
		}
		else {
			pthread_cond_wait(&m_cond, &m_lck);
		}
	}
}

void PlaybackService::setUri(const char* uri)
{
#if 1
	GstPlaybin::getInstance().setUri(uri);
#else
	if(m_multicastcap != NULL) {
		multicastcapture_stop(m_multicastcap);
		multicastcapture_close(m_multicastcap);
		m_multicastcap = NULL;
		pthread_cond_signal(&m_cond);
	}
	multicastcapture_open_param_t param;
	memset(&param, 0, sizeof(param));

#if 0
	sscanf(uri, "rtp://%s:%d", ip, &port);
	param.ip = strdup(ip);
	param.port = port;
#endif
	char ip[64];
	u_int16_t port;
	uri_parse(uri, ip, &port);
	param.ip = strdup(ip);
	param.port = port;
#if 0
	param.cbr.cbr = PlaybackService::dataReceived;
	param.cbr.context = stdout;
#endif
	fprintf(stderr, "!!!!!!!!!!!!! %s : %d\n", param.ip, param.port);
	m_multicastcap = multicastcapture_open(&param);

	char tmp[512];
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "fd://%d", multicastcapture_get_fd(m_multicastcap));
	GstPlaybin::getInstance().setUri(tmp);  //TODO
	pthread_cond_signal(&m_cond);
#endif
}

void PlaybackService::play()
{
	GstPlaybin::getInstance().play();
}

void PlaybackService::stop()
{
	GstPlaybin::getInstance().stop();
}

void PlaybackService::pause()
{
	GstPlaybin::getInstance().pause();
}

void PlaybackService::resume()
{
	GstPlaybin::getInstance().resume();
}

void PlaybackService::setPosition(gint64 position)
{
	GstPlaybin::getInstance().setPosition(position);
}

gint64 PlaybackService::getPosition()
{
	return GstPlaybin::getInstance().getPosition();
}

gdouble PlaybackService::getVolume()
{
	return GstPlaybin::getInstance().getVolume();
}

void PlaybackService::setVolume(gdouble level)
{
	GstPlaybin::getInstance().setVolume(level);
}

gint64 PlaybackService::getDuration()
{
	return GstPlaybin::getInstance().getDuration();
}

gboolean PlaybackService::isPlaying()
{
	return GstPlaybin::getInstance().isPlaying();
}

size_t PlaybackService::dataReceived(void* context, u_int8_t* buf, size_t len)
{
	return fwrite(buf, 1, len, (FILE*) context);
}

void PlaybackService::uri_parse(const char* uri, char* ip, u_int16_t* port)
{
	int rc;
	int i;
	const char* buf = uri;
	stringtokenizer tokenizer = stringtokenizer_new(&buf);
	char tmp[512];

	memset(tmp, 0, sizeof(tmp));
	for(i = 0; (rc = stringtokenizier_next_token(tokenizer, ":/", 2, tmp)) != -1; i++) {
		fprintf(stderr, "[%d] %s\n", i, tmp);
		if(i == 0) {
			// RTP
		}
		else if(i == 3) {
			strcpy(ip, tmp);
		}
		else if(i == 4) {
			*port = atoi(tmp);
		}
		else if(i > 4) {
			break;
		}
	}
	stringtokenizer_delete(tokenizer);
}


} /* namespace halkamalka */
