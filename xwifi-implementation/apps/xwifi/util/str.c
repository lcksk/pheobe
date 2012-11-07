/*
 * str.c
 *
 *  Created on: Oct 17, 2012
 *      Author: buttonfly
 */


#include <stdio.h>
#include <string.h>
#include "os.h"

int str_starts(const char *src, const char *match)
{
	return os_strncmp(src, match, os_strlen(match)) == 0;
}
