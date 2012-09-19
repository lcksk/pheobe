/*
 * ClutterUI.h
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#ifndef CLUTTERUI_H_
#define CLUTTERUI_H_

#include <clutter/clutter.h>
#include <stdlib.h>

namespace halkamalka {

class ClutterUI {
public:
	static ClutterUI& getInstance() {
		static ClutterUI instance;
		return instance;
	}
	void init(ClutterStage* stage);

	static void timelineRotationCompleted(ClutterTimeline* timeline, gpointer user_data);
	static gboolean keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data);

private:
	ClutterUI();
	virtual ~ClutterUI();

	ClutterStage* m_stage;
	ClutterTimeline * m_timeline;
	ClutterBehaviour *m_behaviourOpacity;
	ClutterActor *  m_text;
	bool m_initialized;
};

} /* namespace halkamalka */
#endif /* CLUTTERUI_H_ */
