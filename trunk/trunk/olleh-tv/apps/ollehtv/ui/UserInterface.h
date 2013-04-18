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

namespace halkamalka {

class UserInterface  {
public:
	static UserInterface& getInstance() {
		static UserInterface instance;
		return instance;
	}

	void init();

	static gboolean keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data);
	static void size_change(ClutterActor *texture, gint width, gint height, gpointer user_data);

private:
	UserInterface();
	virtual ~UserInterface();

	pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    ClutterActor* m_stage;
};

} /* namespace halkamalka */
#endif /* USERINTERFACE_H_ */
