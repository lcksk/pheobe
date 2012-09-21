/*
 * subNoti.h
 *
 *  Created on: Sep 21, 2012
 *      Author: buttonfly
 */

#ifndef SUBNOTI_H_
#define SUBNOTI_H_

#include "IsubXXX.h"
#include <clutter/clutter.h>
#include <stdlib.h>

#include "IUpnpDeviceListener.h"

namespace halkamalka {

class subNoti : public IsubXXX, IUpnpDeviceListener {
public:
	subNoti();
	virtual ~subNoti();

	void createPartControl(ClutterActor* stage);
	void setVisible(bool visible);

	static void fadeEffectCompleted(ClutterTimeline* timeline, gpointer data);

	void deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
	void deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);

private:
	ClutterTimeline * m_fadeEffectTimeline;
	ClutterBehaviour *m_behaviourOpacity;
	ClutterActor* m_text;

	bool m_visible;
};

} /* namespace halkamalka */
#endif /* SUBNOTI_H_ */
