/*
 * ServiceManager.cpp
 *
 *  Created on: Sep 30, 2011
 *      Author: buttonfly
 */

#include "ServiceManager.h"
#include <string.h>

namespace halkamalka {

ServiceManager::ServiceManager() {
	pthread_mutex_init(&m_lck, NULL);
	m_workerthread.Init(20);
}

ServiceManager::~ServiceManager() {
	// TODO Auto-generated destructor stub
	 pthread_mutex_destroy(&m_lck);
}

IService* ServiceManager::GetServiceProxy(const char* id)
{
	pthread_mutex_lock(&m_lck);
    for(list<IService*>::iterator pos = m_services.begin(); pos != m_services.end(); pos++)
    {
    	IService* service = dynamic_cast<IService*>(*pos);
        if(service)
        {
        	if(strncmp(service->GetID(), id, strlen(id)) ==0) {
        		 pthread_mutex_unlock(&m_lck);
        		return service;
        	}
        }
    }
    pthread_mutex_unlock(&m_lck);
    return NULL;
}

void ServiceManager::Remove(IService* runnable)
{
	pthread_mutex_lock(&m_lck);
	m_services.remove(runnable);
	pthread_mutex_unlock(&m_lck);
}

void ServiceManager::Invoke(IService* runnable)
{
	pthread_mutex_lock(&m_lck);
	m_workerthread.Invoke(runnable);
	m_services.insert(m_services.end(), runnable);
	pthread_mutex_unlock(&m_lck);
}

} /* namespace halkamalka */
