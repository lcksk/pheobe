/*
 * GstPlaybin.h
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#ifndef GSTPLAYBIN_H_
#define GSTPLAYBIN_H_

#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>
#include <stdlib.h>

namespace halkamalka {

class GstPlaybin {

public:
	static GstPlaybin& getInstance() {
		static GstPlaybin instance;
		return instance;
	}
	void init(ClutterActor* videoTexture);

	int play();
	int stop();
	int pause();
	int resume();
	int setUri(const gchar* uri);

private:
	GstPlaybin();
	virtual ~GstPlaybin();

private:
	ClutterActor *m_videoTexture;
	GstElement *m_sink;
	GstElement * m_playbin;
	GstBus *m_bus;
	bool m_isInitialized;
};

} /* namespace halkamalka */
#endif /* GSTPLAYBIN_H_ */
