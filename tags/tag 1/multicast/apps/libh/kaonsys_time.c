/*
 * kaonsys_time.c
 *
 *  Created on: Sep 28, 2010
 *      Author: buttonfly
 */

#include "kaonsys.h"
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "casdbg.h"
#include <stdlib.h>


#define SYSLOG_
#define TIME_PRINT_
#define S_
#define Q_
#define M_
#define W_
#define ERR
#define DOT_
#define NOT_IMPL
#define H_
#define HEX_

#define knHour2Min(x) 		((U32BIT)(x*60))
#define knDay2SMin(x) 		((U32BIT)(x*60*24))
#define knYear2Min(x) 		((U32BIT)(x*60*24*365))

/**
 * function TIME_GetCurrentTimeOffset
 * @refer to psi_time.c
 */
extern unsigned long Time_GetCurrentTimeOffset();

static U32BIT knOSGetTimeOffset(void);

U32BIT KnOSGetCurrentSystemSecond(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec;
}


void KnOSGetCurrentTimeSpec(_out_ struct KnTimeSpec* spec)
{
	KASSERT(spec);
	struct timeval tv;
	gettimeofday(&tv, NULL);
	spec->sec = tv.tv_sec;
	spec->usec = tv.tv_usec;
}


KnTimeRaw KnOSGetCurrentSystemTime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm* ptm = localtime(&tv.tv_sec);
//	M_("%d %d-%d %d:%d.%d\"", ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	U8BIT year = ptm->tm_year % 10;
	U32BIT temp = ptm->tm_year / 10;
	U8BIT yearOffset = (temp / 10) + (temp % 10);
	U8BIT day = ptm->tm_mday;
	U8BIT month = ptm->tm_mon + 1; // zero base index
	U8BIT hour = ptm->tm_hour;
	U8BIT min = ptm->tm_min;

	// LINUX
	// transform to ADPU TIME format
	U32BIT time = 0;
	U8BIT* time_p = &time;
	time_p[0] = (yearOffset << 5)  & 0xe0;
	time_p[0] |= (day & 0x1f);

	time_p[1] = (month & 0x0f);
	time_p[1] |= (year << 4) & 0xf0;

	time_p[2] = (hour & 0x1f);
	time_p[3] = ((min) & 0x3f);
	return time;
}

// this is temp
KnTimeRaw KnOSGetTDT_Time(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv.tv_sec -= knOSGetTimeOffset(); // offset
	struct tm* ptm = localtime(&tv.tv_sec);

	M_("%d %d-%d %d:%d.%d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	U8BIT year = ptm->tm_year % 10;
	U32BIT temp = ptm->tm_year / 10;
	U8BIT yearOffset = (temp / 10) + (temp % 10);
	U8BIT day = ptm->tm_mday;
	U8BIT month = ptm->tm_mon + 1; // zero base index
	U8BIT hour = ptm->tm_hour;
	U8BIT min = ptm->tm_min;

	// LINUX
	// transform to ADPU TIME format
	U32BIT time = 0;
	U8BIT* time_p = &time;
	time_p[0] = (yearOffset << 5)  & 0xe0;
	time_p[0] |= (day & 0x1f);

	time_p[1] = (month & 0x0f);
	time_p[1] |= (year << 4) & 0xf0;

	time_p[2] = (hour & 0x1f);
	time_p[3] = ((min) & 0x3f);
	return time;
}

U32BIT KnOSGetTDT_Second(void)
{
	return (U32BIT)(KnOSGetCurrentSystemSecond() - knOSGetTimeOffset());
}


U32BIT KnOSEncodeTime(struct KnTime_t* format)
{
	U32BIT time = 0;
	U8BIT* time_p = &time;
	time_p[0] = (format->yearoffset << 5)  & 0xE0;
	time_p[0] |= (format->day & 0x1F);

	time_p[1] = (format->mon & 0x0F);
	time_p[1] |= (format->year << 4) & 0xF0;

	time_p[2] = (format->hour & 0x1F);
	time_p[3] = ((format->min) & 0x3F);
	return time;

}


U32BIT KnOSMktimeSec(const struct KnTime_t* time_p)
{
	struct tm tm;
	time_t time;
	tm.tm_year = 90 + time_p->yearoffset * 10 + time_p->year;//(1990 + (time_p->yearoffset * 10) + time_p->year) - 1900;
	tm.tm_mon = time_p->mon - 1;
	tm.tm_mday = time_p->day;
	tm.tm_hour = time_p->hour;
	tm.tm_min = time_p->min;
	tm.tm_sec = time_p->__sec;
	time = mktime(&tm);
	return time;
}

U32BIT KnOSKnTimeRawToSystemSec(KnTimeRaw time)
{
	struct KnTime_t format;
	KnOSDecodeTime(time, &format);
	return KnOSMktimeSec(&format);
}


U32BIT KnOSDecodeTime(KnTimeRaw time, _out_ struct KnTime_t* format)
{
	U32BIT _time = time;
	U8BIT* time_p = &_time;
	format->yearoffset = ioutil_getbits8(time_p, 5, 3);
	format->year = ioutil_getbits8(&time_p[1], 4, 4);
	format->day = ioutil_getbits8(time_p, 0, 5);
	format->mon = ioutil_getbits8(&time_p[1], 0, 4);
	format->hour = ioutil_getbits8(&time_p[2], 0, 5);
	format->min = ioutil_getbits8(&time_p[3], 0, 6);
	format->__sec = 0;

	M_("%d %d-%d %d:%d", format->year ,  format->mon, format->day, format->hour, format->min);
	return 0;
}


kn_ret_t KnOSTimeSecDiff(U32BIT time1, U32BIT time2, _out_ S32BIT* diff_p)
{
	S_;
	S32BIT diff = 0;
	diff = time2 - time1;
	*diff_p = diff;
	return KN_SUCCESS;
}


kn_ret_t KnOSTimeDiff(KnTimeRaw time1, KnTimeRaw time2, _out_ S32BIT* diff_p)
{
	struct KnTime_t stimes[2];

	KnOSDecodeTime(time1, &stimes[0]);
	KnOSDecodeTime(time2, &stimes[1]);

	S32BIT diff = 0;

#define __CRAP__ do{ 	\
		if(temp != 0)	\
		{\
			if(temp < 0)\
			{\
				*diff_p = -1;\
			}\
			else\
			{\
				*diff_p = 1;\
			}\
			return KN_MUCH_TIME_DIFFERENCE_ERROR;\
		}\
	}while(0)

	U32BIT temp = stimes[1].yearoffset - stimes[0].yearoffset;
	__CRAP__;

	temp = stimes[1].year - stimes[0].year;
	__CRAP__;

	temp = stimes[1].mon - stimes[0].mon;
	__CRAP__;

	temp = stimes[1].day - stimes[0].day;
	__CRAP__;

	temp = stimes[1].hour - stimes[0].hour;
	__CRAP__;
#undef __CRAP__

	if(stimes[1].min != stimes[0].min)
	{
		diff += (stimes[1].min - stimes[0].min);
	}

	*diff_p = diff;
	return KN_SUCCESS;
}

static U32BIT knOSGetTimeOffset(void)
{
	long offsetMinutes = Time_GetCurrentTimeOffset();
	return (offsetMinutes * 60);
}
