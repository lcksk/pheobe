/*
 * Platform.h
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <clutter/clutter.h>
#include <stdlib.h>

namespace halkamalka {

class Platform {

public:
	static Platform& getInstance() {
		static Platform instance;
		return instance;
	}

	void init(int* argc, char*** argv);
	gint start(void);

	void* getWindow() const;

	static void windowSizeChanged (ClutterStage * stage, gpointer data);
	static gboolean stageEvent(ClutterStage * stage, ClutterEvent * event, gpointer ui);

private:
	Platform();
	virtual ~Platform();

	bool m_initialized;
	ClutterActor *m_stage;
	ClutterActor* m_videoTexture;
};

} /* namespace halkamalka */
#endif /* PLATFORM_H_ */
