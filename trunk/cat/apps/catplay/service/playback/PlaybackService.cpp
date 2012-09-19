/*
 * PlaybackService.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#include "PlaybackService.h"
#include "GstPlaybin.h"

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
		pthread_cond_wait(&m_cond, &m_lck);
	}
}

void PlaybackService::setUri(const char* uri)
{
	GstPlaybin::getInstance().setUri(uri);
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

} /* namespace halkamalka */
