/*
 * PlaybackService.h
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#ifndef PLAYBACKSERVICE_H_
#define PLAYBACKSERVICE_H_

#include "IService.h"
#include <pthread.h>
#include <glib.h>
#include "multicastcapture.h"

namespace halkamalka {

class PlaybackService : public halkamalka::IService {
public:
	const static char* ID;

	PlaybackService();
	virtual ~PlaybackService();

	void Run();
	const char* GetID();

	void setUri(const char* uri);
	void play();
	void stop();
	void pause();
	void resume();

	void setPosition(gint64 position);
	gint64 getPosition();
	gdouble getVolume();
	void setVolume(gdouble level);
	gint64 getDuration();
	gboolean isPlaying();

private:
	static size_t dataReceived(void* context, u_int8_t* buf, size_t len);
	void uri_parse(const char* uri, char* ip, u_int16_t* port);
    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;
    multicastcapture		m_multicastcap;
};

} /* namespace halkamalka */
#endif /* PLAYBACKSERVICE_H_ */
