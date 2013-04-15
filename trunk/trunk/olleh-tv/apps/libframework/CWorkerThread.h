/*
 * CWorkerThread.h
 *
 *  Created on: Apr 17, 2009
 *      Author: buttonfly
 */

#ifndef CWORKERTHREAD_H_
#define CWORKERTHREAD_H_

#include "IRunnable.h"
#include <pthread.h>
#include <list>

using namespace std;

namespace halkamalka {

class CWorkerThread {
public:
    CWorkerThread();
    virtual ~CWorkerThread();

    void Init(unsigned int numofThread);
    void Deinit(void);

    void Invoke(IRunnable* runnable);

private:
    static void* work(CWorkerThread* worker);
    IRunnable* getIRunnable(void);

private:
    list<IRunnable*>	m_runnables;
    list<pthread_t>		m_threads;
    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    bool		m_initialized;
    bool		m_deinitialized;
    bool		m_closed;
};

}

#endif /* CWORKERTHREAD_H_ */
