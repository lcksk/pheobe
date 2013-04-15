



/*
 * CWorkerThread.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: buttonfly
 */

#include "CWorkerThread.h"

namespace halkamalka {

CWorkerThread::CWorkerThread()
: m_initialized(false), m_deinitialized(false), m_closed(false)
{
}

CWorkerThread::~CWorkerThread()
{
    Deinit();
}

void CWorkerThread::Invoke(IRunnable* runnable)
{
    pthread_mutex_lock(&m_lck);
    m_runnables.insert(m_runnables.end(), runnable);
    pthread_cond_broadcast(&m_cond);
    pthread_mutex_unlock(&m_lck);
}


IRunnable* CWorkerThread::getIRunnable(void)
{
    pthread_mutex_lock(&m_lck);
    for(;;)
    {
        if(m_runnables.size() > 0)
        {
            break;
        }
        pthread_cond_wait(&m_cond, &m_lck);
        if(m_closed)
        {
            pthread_mutex_unlock(&m_lck);
            pthread_exit((void*) NULL);
        }
    }

    IRunnable* runnable = NULL;
    list<IRunnable*>::iterator pos = m_runnables.begin();
    runnable = (IRunnable*) *pos;
    *pos = NULL;

    m_runnables.remove(NULL);
    pthread_mutex_unlock(&m_lck);
    return runnable;
}



void* CWorkerThread::work(CWorkerThread* worker)
{
    while(worker->m_closed != true)
    {
        IRunnable* runnable = worker->getIRunnable();
        if(runnable)
        {
            runnable->Run();
        }
    }
    pthread_exit((void*) NULL);
    return NULL;
}

void CWorkerThread::Init(unsigned int numofThread)
{
    if(m_initialized == false)
    {
        pthread_attr_t attri;
        pthread_attr_init(&attri);
        pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

        pthread_mutex_init(&m_lck, NULL);
        pthread_cond_init(&m_cond, NULL);

        pthread_t thread;
        for(int i = 0; i < (int)numofThread; i++)
        {
            pthread_create(&thread, &attri, (void*(*)(void*))CWorkerThread::work, (void*)this);
            m_threads.insert(m_threads.end(), thread);
        }
        pthread_attr_destroy(&attri);
        m_initialized = true;
    }
}


void CWorkerThread::Deinit(void)
{
    if(m_deinitialized == false)
    {
        pthread_mutex_lock(&m_lck);
        m_closed = true;
        pthread_cond_broadcast(&m_cond);
        pthread_mutex_unlock(&m_lck);

        pthread_t thread;
        for(list<pthread_t>::iterator  pos = m_threads.begin(); pos != m_threads.end(); pos++)
        {
            thread = static_cast<pthread_t> (*pos);
            pthread_join(thread, NULL);
        }

        for(list<IRunnable*>::iterator pos = m_runnables.begin(); pos != m_runnables.end(); pos++)
        {
            IRunnable* runnable = dynamic_cast<IRunnable*>(*pos);
            if(runnable)
                m_runnables.remove(runnable);
        }

        pthread_mutex_destroy(&m_lck);
        pthread_cond_destroy(&m_cond);

        m_deinitialized = true;
    }
}


#if this_is_example
/*
 * main.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: buttonfly
 */

#include "CWorkerThread.h"
#include "CTestClass.h"
#include "constant.h"

using namespace halkamalka;

int main(int argc, char** argv)
{
    CWorkerThread thread;
    thread.Init(10);

    IRunnable* runnable = new CTestClass;
    while(getchar())
    {
        TRD_MSG("__");
        thread.Invoke(runnable);
    }
    return 0;
}
#endif

}
