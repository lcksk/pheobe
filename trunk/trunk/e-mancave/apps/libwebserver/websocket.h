/*
 * Copyright (c) 2010 Putilov Andrey
 *
 * Permission is hereby granted, free of uint8_tge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef WEBSOCKET_H
#define	WEBSOCKET_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdint.h> /* uint8_t */
#include <stdlib.h> /* strtoul */
#include <string.h>
#include <stdio.h> /* sscanf */
#include <ctype.h> /* isdigit */
#include <stddef.h> /* size_t */
#include <poll.h>
#include "md5.h"
#ifdef __AVR__
	#include <avr/pgmspace.h>
#else
	#define PROGMEM
	#define PSTR
	#define strstr_P strstr
	#define sscanf_P sscanf
	#define sprintf_P sprintf
	#define strlen_P strlen
	#define memcmp_P memcmp
#endif

#include "mongoose.h"

enum ws_frame_type {
	WS_ERROR_FRAME,
	WS_INCOMPLETE_FRAME,
	WS_TEXT_FRAME,
	WS_BINARY_FRAME,
	WS_OPENING_FRAME,
	WS_CLOSING_FRAME
};

typedef enum {
    hybi_00, /* Hixie_76 */
    hybi_08, /* 5 through to 13 */

} ws_protocol_t;

struct ws_info {
	char            *resource;
	char            *host;
	char            *origin;
	char            *protocol;
	char            *key1;
	char            *key2;
	char            key3[8];
	ws_protocol_t   version;
	int             fd;
};

#define MAX_KEY_LEN 128

int mg_ws_parse_header(struct ws_info *wsi, int fd, struct mg_request_info *ri, char* buf, int length);

/* This function will receive data from the Websocket connection and then modify
 * the pointer to strip out the header and footer.
 */
int mg_ws_receive(struct ws_info *wsi, char *buf, int length, char** in_data, int *data_length, enum ws_frame_type *type);


int mg_ws_write(struct ws_info *wsi, const char *data, int data_len);

#ifdef	__cplusplus
}
#endif

#endif	/* WEBSOCKET_H */

