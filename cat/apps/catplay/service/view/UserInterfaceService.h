/*
 * UserInterfaceService.h
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#ifndef USERINTERFACESERVICE_H_
#define USERINTERFACESERVICE_H_

#include "IService.h"
#include <pthread.h>
#include <clutter/clutter.h>
#include <stdlib.h>
#include "IUpnpDeviceListener.h"

namespace halkamalka {

class UserInterfaceService : public halkamalka::IService, halkamalka::IUpnpDeviceListener  {
public:
	const static char* ID;

	UserInterfaceService();
	virtual ~UserInterfaceService();

	void Run();
	const char* GetID();

	void deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
	void deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);

	static gboolean keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data);
	static void fadeEffectCompleted(ClutterTimeline* timeline, gpointer data);

private:
    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    ClutterActor* m_stage;
    ClutterTimeline * m_fadeEffectTimeline;
    ClutterBehaviour *m_behaviourOpacity;
    ClutterActor* m_text;
};

} /* namespace halkamalka */
#endif /* USERINTERFACESERVICE_H_ */
