/*
 * kaonsys_mem.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include "kaonsys.h"
#include <string.h>

void* KnCalloc(U32BIT count, U32BIT size)
{
	return calloc(count, size);
}

void* KnMalloc(U32BIT size)
{
	void* ptr = malloc(size);
	if(ptr != NULL)
	{
		KnMemset(ptr, 0, size);
	}
	return ptr;
}

void KnFree(void* ptr)
{
	if(ptr != NULL)
	{
		free(ptr);
	}
}

void KnMemcpy(void* dst, const void* src, U32BIT len)
{
	memcpy(dst, src, len);
}

void KnMemset(void* dst, U32BIT v, U32BIT size)
{
	memset(dst, v, size);
}

U32BIT KnMemcmp(void* dst, const void* src, U32BIT len)
{
	return memcmp(dst, src, len);
}
