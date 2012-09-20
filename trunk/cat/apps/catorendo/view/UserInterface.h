/*
 * UserInterface.h
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#ifndef USERINTERFACE_H_
#define USERINTERFACE_H_

#include <pthread.h>
#include <clutter/clutter.h>
#include <stdlib.h>
#include "IUpnpDeviceListener.h"

namespace halkamalka {

class UserInterface : halkamalka::IUpnpDeviceListener  {
public:
	static UserInterface& getInstance() {
		static UserInterface instance;
		return instance;
	}

	void init();

	void deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
	void deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);

	static gboolean keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data);
	static void fadeEffectCompleted(ClutterTimeline* timeline, gpointer data);

private:
	UserInterface();
	virtual ~UserInterface();

	pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    ClutterActor* m_stage;
    ClutterTimeline * m_fadeEffectTimeline;
    ClutterBehaviour *m_behaviourOpacity;
    ClutterActor* m_text;
};

} /* namespace halkamalka */
#endif /* USERINTERFACE_H_ */
