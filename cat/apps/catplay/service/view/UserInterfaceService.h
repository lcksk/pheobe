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

namespace halkamalka {

class UserInterfaceService : public halkamalka::IService  {
public:
	enum {
		VK_UP = 111,
		VK_LEFT = 113,
		VK_RIGHT = 114,
		VK_DOWN = 116,
		VK_ENTER = 36,
		VK_SPACE_BAR = 65
	};

	const static char* ID;

	UserInterfaceService();
	virtual ~UserInterfaceService();

	void Run();
	const char* GetID();

	static gboolean keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data);
	static void windowSizeChanged (ClutterStage * stage, gpointer data);

private:
    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    ClutterActor* m_stage;
};

} /* namespace halkamalka */
#endif /* USERINTERFACESERVICE_H_ */
