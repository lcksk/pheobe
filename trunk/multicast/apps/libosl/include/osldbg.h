/*
 * kutil.h
 *
 *  Created on: Apr 7, 2011
 *      Author: buttonfly
 */

#ifndef OSLDBG_H_
#define OSLDBG_H_


#ifdef __cplusplus
extern "C"
{
#endif


#define _out_

#define _TEMP_DBG_

#ifdef _TEMP_DBG_
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>



#define SYSLOG_(fmt, args...) do{syslog(LOG_INFO, "[IPTV]"fmt, ##args);}while(0)
#define TIME_PRINT	do{struct timeval tv; gettimeofday(&tv, NULL);fprintf(stderr, "[%04d.%06d]", (int)tv.tv_sec, (int)tv.tv_usec);}while(0)
#define S_			do{TIME_PRINT;fprintf(stderr, "[%8x]S: %s[%d] \n", (int)pthread_self(), __PRETTY_FUNCTION__, __LINE__);}while(0)
#define E_			do{TIME_PRINT;fprintf(stderr, "[%8x]F: %s[%d] \n", (int)pthread_self(), __FUNCTION__, __LINE__);}while(0)
#define Q_ do{TIME_PRINT; fprintf(stderr, "[%8x]Q_[ %s[%d]\n", (int)pthread_self(),__FUNCTION__, __LINE__);}while(0)
#define M_(fmt,args...) do{TIME_PRINT; fprintf(stderr, "[%8x]M: "fmt,(int)pthread_self(),##args);fprintf(stderr, "|| %s[L:%d]\n",__FUNCTION__, __LINE__);}while(0)
#define P_(fmt,args...) do{fprintf(stderr,##args);}while(0);
#define W_(fmt,args...) do{TIME_PRINT; fprintf(stderr, "[%8x]W: "fmt,(int)pthread_self(),##args);fprintf(stderr, "|| %s[L:%d]\n",__FUNCTION__, __LINE__);}while(0)
#define ERR(fmt,args...) do{TIME_PRINT; fprintf(stderr, "[%8x]E: "fmt,(int)pthread_self(),##args);fprintf(stderr,"|| %s::%s[L:%d]\n", __FILE__, __FUNCTION__, __LINE__);}while(0)
#define DOT_ fprintf(stderr, ".");
#define NOT_IMPL do{fprintf(stderr, "[%8x] NOT_IMPL!!!!!!!!!!! %s[%d]\n", (int)pthread_self(), __FUNCTION__, __LINE__); return -1;}while(0)
#define H_(buffer, buffer_len)	 do{	\
										int _kn_; 	\
										fflush(stderr); \
										for(_kn_ = 0 ; _kn_ < buffer_len; _kn_++){	\
											if((_kn_ != 0) && ((_kn_ %0x10) == 0)){	\
												fprintf(stderr, "\n");	\
											}	\
											fprintf(stderr, "%02x ", buffer[_kn_]);	\
										}	\
										fprintf(stderr, "\n");	\
									}while(0)

#else
#define SYSLOG_
#define TIME_PRINT_
#define S_
#define E_
#define Q_
#define M_
#define W_
#define ERR
#define DOT_
#define NOT_IMPL
#define H_
#endif



#if 0
#include <stdlib.h>
#define KASSERT
#else
#include <stdlib.h>
#define KASSERT(x)	do{ if(!(x)){ERR("Assertion failed in line"); abort(); }; }while(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* OSLDBG_H_ */
