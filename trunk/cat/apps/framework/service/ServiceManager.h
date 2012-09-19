/*
 * ServiceManager.h
 *
 *  Created on: Sep 30, 2011
 *      Author: buttonfly
 */

#ifndef SERVICEMANAGER_H_
#define SERVICEMANAGER_H_

#include "IRunnable.h"
#include "CWorkerThread.h"
#include "IService.h"

using namespace halkamalka;

namespace halkamalka {

class ServiceManager {
public:
	static ServiceManager& GetInstance()
	{
		static ServiceManager instance;
		return instance;
	}

	IService* GetServiceProxy(const char* id);

	void Invoke(IService* runnable);

	void Remove(IService* runnable);

private:
	ServiceManager();
	virtual ~ServiceManager();

	CWorkerThread m_workerthread;
	list<IService*> m_services;
    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;


};

} /* namespace kn */
#endif /* SERVICEMANAGER_H_ */
