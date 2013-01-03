/*
 * kaonsys.h
 *
 *  Created on: Sep 10, 2010
 *      Author: buttonfly
 */

#ifndef KAONSYS_H_
#define KAONSYS_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef unsigned char		U8BIT;
typedef unsigned short 	U16BIT;
typedef unsigned long		U32BIT;

typedef char		S8BIT;
typedef short 	S16BIT;
typedef long		S32BIT;


#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif


typedef enum
{
	KN_SUCCESS,
	KN_ERROR_OUT_OF_MEMORY,
	KN_ERROR_INVALID_PARAM,
	KN_NOT_SUPPORTED,
	KN_NOT_AVAILABLE,
	KN_ERROR_BUSY,
	KN_EXTERNAL_ERROR,
	KN_INVALID_STATES,
	KN_TIMEOUT,
	KN_RESET_REQUIRED,
	KN_FAILTURE,
	KN_MUCH_TIME_DIFFERENCE_ERROR,
	KN_SMARTCARD_ERROR,
	KN_SMARTCARD_ERROR_RX_PARITY,
	KN_SMARTCARD_ERROR_TX_PARITY,
	KN_SMARTCARD_ERROR_RX_TIMEOUT,
	KN_SMARTCARD_ERROR_TX_TIMEOUR,
	KN_SMARTCARD_ERROR_HW_FAILURE,
	KN_SMARTCARD_RESET_TERMINAL, //requires a call to bsmartcard_reset()
} kn_ret_t;

typedef enum
{
	FALSE,
	TRUE,
} BOOLEAN;
typedef void* KnHandle;
typedef void* KnMutex_t;
typedef void* KnThread_t;
typedef void* KnSemaphore_t;
typedef void* KnQueue_t;
typedef void* KnFlash_t;

#define KN_INFINITE		0xFFFF
#define KN_TIMEOUT		20
#define _out_

unsigned int KnOSGetClockMilliseconds(void);

/**
 * @function	KnOSCreateQueue
 * @param 	msgSize
 * @param	msgMax
 * @return	handle which queue has been created
 * @description
 * All fucntion are of thread-safety
 */
KnQueue_t KnOSCreateQueue(U16BIT msgSize, U16BIT msgMax);

/**
 * @function KnOSDestroyQueue
 * @param 	queue			Q handle
 * @return
 */
BOOLEAN KnOSDestroyQueue(KnQueue_t queue);

/**
 * @function KnOSReadQueue
 * @param	queue			Q handle
 * @param 	msg				pointer of buffer
 * @param	msg_size		size to read
 * @param	timeout			time out @see also: KN_INFINITE
 * @return						TRUE: success to read, FALSE: fail to read or time out
 * @description
 * If KN_INFINITE value is set, It will wait infinitely
 */
BOOLEAN KnOSReadQueue(KnQueue_t queue, void* msg, U16BIT msg_size, U16BIT timeout);

/**
 * @function KnOSWriteQueue
 * @param	queue			Q handle
 * @param	msg				pointer of buffer to send
 * @param	msg_size		size to be read
 * @param	timeout			meaningless here
 * @return
 */
BOOLEAN KnOSWriteQueue(KnQueue_t queue, void* msg, U16BIT msg_size, U16BIT timeout);

BOOLEAN KnOSRemoveAllQueue(KnQueue_t queue);

/**
 * @function KnOSCreateTask
 * @param	function		pointer function
 * @param 	param			argument to be aligned when the [fucntion] is called
 * @param	stack			the specific stack size. if you set 0, it would be set as default stack size
 * @param	name			@deprecated
 * @return						handle
 */
KnThread_t KnOSCreateTask(void (*function)(void*), void* param, U32BIT stack, U8BIT priority, U8BIT* name);

/**
 * @function KnOSTaskDelay
 * @param	timeout			millisecond
 */
void KnOSTaskDelay (U32BIT timeout);

/**
 * @function	KnOSTaskSleep
 * @param	timeout			second
 */
void KnOSTaskSleep(U32BIT timeout);

/**
 * @function	KnOSGetCurrentTask
 * @return						current thread
 */
KnThread_t KnOSGetCurrentTask(void);

/**
 * @function	KnOSCreateSemaphore
 * @return	semaphore handle
 */
KnSemaphore_t KnOSCreateSemaphore(void);

/**
 * @function KnOSCreateCountSemaphore
 * @param 	value
 */
KnSemaphore_t KnOSCreateCountSemaphore(U32BIT value);

/**
 * @function KnOSInitCountSemaphore
 * @param	semaphore	semaphore handle
 * @param	value
 * @deprecated		It is used internally
 */
void KnOSInitCountSemaphore(KnSemaphore_t semaphore, U32BIT value);

/**
 * @function	KnOSDeleteSemaphore
 * @param	semaphore
 * @description
 */
void KnOSDeleteSemaphore(KnSemaphore_t semaphore);

/**
 * @function	KnOSSemaphoreWait
 * @param	semaphore
 * @description
 * The running status if the current thread is changed to the status of NOT_RUNNABLE
 */
void KnOSSemaphoreWait(KnSemaphore_t semaphore);

/**
 * @function KnOSSemaphoreSignal
 * @param	semaphore
 * @description
 * wake up
 */
void KnOSSemaphoreSignal(KnSemaphore_t semaphore);

/***
 * @function	KnOSSemaphoreWaitTimeout
 * @param	semaphore
 * @param	timeout				milli
 * @return
 */
BOOLEAN KnOSSemaphoreWaitTimeout (KnSemaphore_t semaphore, U16BIT timeout);

/**
 * @function	KnOSCreateMutex
 * @return	mutex handle
 */
KnMutex_t KnOSCreateMutex (void);

/**
 * @function	KnOSMutexLock
 * @param	handle
 */
void KnOSMutexLock (KnMutex_t mutex);

/**
 * @function	KnOSMutexUnlock
 * @param	handle
 */
void KnOSMutexUnlock (KnMutex_t mutex);

/**
 * @function	KnOSMutexUnlock
 * @param	handle
 */
void KnOSDeleteMutex (KnMutex_t mutex);

/**
 * @function KnCalloc
 * @param count
 * @param size
 * @return
 */
void* KnCalloc(U32BIT count, U32BIT size);

/**
 * @function KnMalloc
 * @param size
 * @return
 */
void* KnMalloc(U32BIT size);

/**
 * @function KnFree
 * @param ptr
 */
void KnFree(void* ptr);

/**
 * @function KnMemcpy
 * @param dst
 * @param src
 * @param len
 */
void KnMemcpy(void* dst, const void* src, U32BIT len);

/**
 * @function KnMemset
 * @param dst
 * @param v
 * @param size
 */
void KnMemset(void* dst, U32BIT v, U32BIT size);

/**
 * @function KnMemcmp
 * @param dst
 * @param src
 * @param len
 * @return
 */
U32BIT KnMemcmp(void* dst, const void* src, U32BIT len);

// FLASH
typedef enum
{
	eFLASH_FingerPrint,
	eFLASH_UserText,
	eFLASH_pincode,
	eFLASH_Subscriptions,
	eFLASH_Events,
	eFLASH_TokenStatuses,
	eFLASH_PurseDebits,
	eFLASH_PurseCredits,
	eNUMOF_FLASH_FILE
} eConax_FlashBlock;


typedef enum
{
	eSEEK_SET = 0,
	eSEEK_CUR,
	eSEEK_END,
} eSEEK_t;

#define KN_SEEK_SET				0
#define KN_SEEK_CUR				1
#define KN_SEEK_END			2


/**
 * @function KnOSUnlinkFile
 * @param handle
 */
void KnOSUnlinkFile(KnFlash_t handle);
/**
 * @function KnOSCreateFile
 * @param block
 * @return
 */
KnFlash_t KnOSCreateFile(eConax_FlashBlock block);

/**
 * @function KnOSReadFile
 * @param handle
 * @param msg
 * @param msg_size
 * @return
 */
S32BIT KnOSReadFile(KnFlash_t handle, void* msg, U32BIT msg_size);

/**
 * @function KnOSWriteFile
 * @param handle
 * @param msg
 * @param msg_size
 * @return
 */
S32BIT KnOSWriteFile(KnFlash_t handle, void* msg, U32BIT msg_size);

/**
 * @function KnOSGetSizeFile
 * @param handle
 * @return
 */
S32BIT KnOSGetSizeFile(KnFlash_t handle);

/**
 * @function KnOSSeekFile
 * @param handle
 * @param offset
 * @param whence
 * @return
 */
S32BIT KnOSSeekFile(KnFlash_t handle, S32BIT offset, eSEEK_t whence);

/**
 * @function KnOSCloseFile
 * @param handle
 */
void KnOSCloseFile(KnFlash_t);

/**
 * @function KnOSRequestFlashAddress
 * @param handle
 * @param size
 * @return
 */
U8BIT* KnOSRequestFlashAddress(KnFlash_t handle, S32BIT size);

/**
 * @function KnOSReleaseFlashAddress
 * @param addr
 * @param size
 * @return
 */
S32BIT KnOSReleaseFlashAddress(U8BIT* addr, S32BIT size);


/**
 * @KnTimeRaw
 * year_offset : 3	uimsbf
 * day			: 5	uimsbf
 * year			: 4	uimsbf
 * month		: 4	uimsbf
 * unused		: 3	bslbf
 * hour			: 5	uimsbf
 * unused		: 2	bslbf
 * minute		: 6	uimsbf
 */
typedef U32BIT		KnTimeRaw;

struct KnTimeSpec
{
	S32BIT					sec;
	S32BIT					usec;
};
void KnOSGetCurrentTimeSpec(_out_ struct KnTimeSpec* spec);


// APDU TIME FORMAT
struct KnTime_t
{
	U8BIT					yearoffset;
	U32BIT					year;
	U32BIT					day;
	U32BIT					mon;
	U32BIT					hour;
	U32BIT					min;
	U8BIT					len;
	U32BIT					__sec;
};

/**
 * @function KnOSMktimeSec
 * @return second
 * @description
 */
U32BIT KnOSMktimeSec(const struct KnTime_t* time);

/**
 * @function
 */
U32BIT KnOSKnTimeRawToSystemSec(KnTimeRaw time);

/**
 * @
 * @param time1 APDU TIME Format
 * @param time2 APDU TIME Format
 * @return KN_SUCCESS | KN_MUCH_TIME_DIFFERENCE_ERROR
 * @description This function is valid only if the time-difference is under a day.
 * if not, it will return KN_MUCH_TIME_DIFFERENCE_ERROR.
 * Make sure that it returns in minutes, NOT SECOND!
 */
kn_ret_t KnOSTimeDiff(KnTimeRaw time1, KnTimeRaw time2, _out_ S32BIT* diff);

/**
 * @function KnOSTimeSecDiff
 * @param time1
 * @param time2
 * @param diff
 * @return
 */
kn_ret_t KnOSTimeSecDiff(U32BIT time1, U32BIT time2, _out_ S32BIT* diff);

/**
 * @function KnOSGetCurrentSystemSecond
 * @return
 */
U32BIT KnOSGetCurrentSystemSecond(void);
/**
 * @function KnOSGetCurrentSystemTime
 * @return 	KnTimeRaw @see also: APDU TIME FORMAT in CONAX spec.
 * @description
 * It returns system time in unit: minute.
 * private note: This function is written to compare with time of USER_TEXT.
 */
KnTimeRaw KnOSGetCurrentSystemTime(void);

/**
 * @function KnOSGetTDT_Time
 * @return
 */
KnTimeRaw KnOSGetTDT_Time(void);

/**
 * @function KnOSGetTDT_Second
 * @return
 */
U32BIT KnOSGetTDT_Second(void);

/**
 * @function KnOSDecodeTime
 * @param	time	APDU TIME Format
 * @param	format	@see also: struct KnTime_t
 * @return	meaningless
 */
U32BIT KnOSDecodeTime(KnTimeRaw time, _out_ struct KnTime_t* format);

/**
 * @function KnOSEncodeTime
 * @param time APDU TIME FORMAT
 * @param format out parameter
 * @return
 */
KnTimeRaw KnOSEncodeTime(struct KnTime_t* format);



#define KN_RDONLY				O_RDONLY
#define KN_WRONLY				O_WRONLY
#define KN_RDWR					O_RDWR
#define KN_NOCTTY					O_NOCTTY
#define KN_TRUNC					O_TRUNC
#define KN_NONBLOCK			O_NONBLOCK
#define KN_DIRECT					00040000
#define KN_LARGEFILE			00100000
#define KN_DIRECTORY			00200000
#define KN_NOFOLLOW			00400000
#define KN_NOATIME				01000000

#define KN_APPEND				O_APPEND//0x0008
#define KN_SYNC					O_SYNC//0x0010
#define KN_CREAT         			O_CREAT//0x0100	/* not fcntl */
#define KN_EXCL						O_EXCL//0x0400	/* not fcntl */

typedef struct
{
	KnHandle (*_open)(S8BIT*, U32BIT);
} KnFileop_t;


void KnUnlink(S8BIT* devicename);
/**
 * @function KnOSOpen
 * @param devicename
 * @param option
 * @return
 */
KnHandle KnOSOpen(S8BIT* devicename, U32BIT option);

/**
 * @function KnOSRead
 * @param handle
 * @param buffer
 * @param nread
 * @return
 */
S32BIT	KnOSRead(KnHandle handle, _out_ U8BIT* buffer, U32BIT nread);

/**
 * @function KnOSWrite
 * @param handle
 * @param buffer
 * @nwrite
 * @return
 */
S32BIT	KnOSWrite(KnHandle handle, const U8BIT* buffer, U32BIT nwrite);

/**
 * @function KnOSClose
 * @param handle
 */
void KnOSClose(KnHandle handle);

/**
 * @deprecated
 * @function KnOSGetSize
 * @param handle
 * @return
 */
S32BIT KnOSGetSize(KnHandle handle);

/**
 * @function KnOSSeek
 * @param handle
 * @param offset
 * @param whence
 * @return
 */
S32BIT	KnOSSeek(KnHandle handle, U32BIT offset, eSEEK_t whence);

/**
 * @function KnOSRequestAddress
 * @param handle
 * @param size
 * @return
 */
U8BIT* KnOSRequestAddress(KnHandle handle, S32BIT size);

/**
 * @function KnOSReleaseAddress
 * @param addr
 * @param size
 * @return
 */
S32BIT KnOSReleaseAddress(U8BIT* addr, S32BIT size);

/**
 * @function KnOS_Encrypt
 * @param src
 * @param dst
 * @param len
 * @description The key is the hidden key which STB provides.
 */
kn_ret_t KnOS_Encrypt(U8BIT* src, U8BIT* dst, U32BIT len);

/**
 * @function KnOS_Decrypt
 * @param src
 * @param dst
 * @param len
 * @description The key is the hidden key which STB provides.
 */
kn_ret_t KnOS_Decrypt(U8BIT* src, U8BIT* dst, U32BIT len);


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


#endif /* KAONSYS_H_ */
