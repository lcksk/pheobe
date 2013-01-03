/*
 * kaonsys_vfs.c
 *
 *  Created on: Sep 30, 2010
 *      Author: buttonfly
 */

#include "kaonsys.h"

#include "ioutil.h"
#include "casdbg.h"
#include <mtd/mtd-user.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

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

// FILESYSTEM MOUNTED

#define LOW_LEVEL_FILE

KnHandle KnOSOpen(S8BIT* devicename, U32BIT option)
{
	S_;
	M_("%s", devicename);
	S32BIT fd = open(devicename, option);
	M_("fd: %d", fd);
	if(fd > 0)
	{
		Q_;
		return fd;
	}
	return NULL;
}

S32BIT	KnOSRead(KnHandle handle, _out_ U8BIT* buffer, U32BIT msg_size)
{
	S_;
	S32BIT offset = 0;
	S32BIT nread = 0;

	return read(handle, buffer, msg_size);
//	for(;offset < msg_size; offset += nread)
//	{
//		nread = read(handle, buffer, msg_size - offset);
//	}
//	return offset;
}

S32BIT	KnOSWrite(KnHandle handle, const U8BIT* buffer, U32BIT msg_size)
{
	S_;
	S32BIT offset = 0;
	S32BIT nwrite = 0;

	return write(handle, buffer, msg_size);
//	for(;offset < msg_size; offset += nwrite)
//	{
//		nwrite = write(handle, buffer, msg_size - offset);
//	}
//	return offset;
}

void KnOSClose(KnHandle handle)
{
	S_;
	close(handle);
}

S32BIT KnOSGetSize(KnHandle handle)
{
	KASSERT(handle);
	struct stat st;
	KnMemset(&st, 0, sizeof(st));
	S32BIT ret = fstat(handle, &st);
	if(ret < 0)
	{
		return -1;
	}
	return st.st_size;
}

S32BIT	KnOSSeek(KnHandle handle, U32BIT offset, eSEEK_t whence)
{
	return lseek(handle, offset, whence);
}

U8BIT* KnOSRequestAddress(KnHandle handle, S32BIT size)
{
	U8BIT* map_p;
	KnOSSeek(handle, size , eSEEK_SET);
	map_p = mmap(0, size, PROT_WRITE | PROT_READ, MAP_PRIVATE,  handle, 0);
	if(MAP_FAILED == map_p)
	{
		Q_;
		return NULL;
	}
	return map_p;
}

S32BIT KnOSReleaseAddress(U8BIT* addr, S32BIT size)
{
	return munmap(addr, size);
}

void KnUnlink(S8BIT* devicename)
{
	unlink(devicename);
}

