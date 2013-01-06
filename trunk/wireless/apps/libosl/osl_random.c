/*
 * osl_random.c
 *
 *  Created on: Jan 4, 2013
 *      Author: buttonfly
 */

#include "osl.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

osl_random osl_random_open()
{
	int fd = open("/dev/random", O_RDONLY);
	if(fd == -1)
		return NULL;

	return (osl_random) fd;
}

u_int32_t osl_random_read(osl_random random, u_int8_t* buf, u_int32_t len)
{
	int nread;
	nread = read((int)random, buf, len);
	return nread;
}

void osl_random_close(osl_random random)
{
	close((int)random);
}


