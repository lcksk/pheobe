/*
 * playback.h
 *
 *  Created on: Sep 12, 2012
 *      Author: buttonfly
 */

#ifndef PLAYBACK_H_
#define PLAYBACK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "glib.h"

typedef void* playback;

playback playback_getInstance();

int playback_set_uri(playback handle, const gchar* uri);

int playback_play(playback handle, gint speed);

int playback_stop(playback handle);

int playback_pause(playback handle);

int playback_resume(playback handle);

#ifdef __cplusplus
}
#endif


#endif /* PLAYBACK_H_ */
