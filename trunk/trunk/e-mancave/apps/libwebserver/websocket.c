/*
 * Copyright (c) 2010 Putilov Andrey
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
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

#include "websocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 1
#endif

#define BUF_LEN 0x1FF

#define ERR(x) fprintf(stderr, "%s()[%d]: %s (%s)\n", __FUNCTION__, __LINE__, #x, strerror(errno));

#define input_ptr_len (input_len - (input_ptr-input_frame))
#define prepare(x) do {if (x) { free(x); x = NULL; }} while(0)

static const char ws_header_connection[]     = "Connection";
static const char ws_header_upgrade[]        = "Upgrade";
static const char ws_header_origin[]         = "Origin";
static const char ws_header_host[]           = "Host";
static const char ws_header_sec_origin[]     = "Sec-WebSocket-Origin";
static const char ws_header_sec_protocol[]   = "Sec-WebSocket-Protocol";
static const char ws_header_sec_key[]        = "Sec-WebSocket-Key";
static const char ws_header_sec_key1[]       = "Sec-WebSocket-Key1";
static const char ws_header_sec_key2[]       = "Sec-WebSocket-Key2";
static const char ws_header_sec_version[]    = "Sec-WebSocket-Version";
static const char ws_header_sec_location[]   = "Sec-WebSocket-Location";
static const char ws_header_websocket[]      = "WebSocket";
static const char ws_header_websocket_s[]    = "webSocket";
static const char ws_header_hixie_resp[]     = "HTTP/1.1 101 WebSocket Protocol Handshake";
static const char ws_header_hybi_resp[]      = "HTTP/1.1 101 Switching Protocols";
static const char ws_header_sec_accept[]     = "Sec-WebSocket-Accept";

static const char rn[] PROGMEM               = "\r\n";

extern int lws_b64_encode_string(const char *in, int in_len, char *out, int out_size);

extern int lws_b64_decode_string(const char *in, char *out, int out_size);
extern unsigned char* SHA1(const unsigned char *d, size_t n, unsigned char *md);


/* Hexie-76/Hybi-00 Protocol */
static uint32_t doStuffToObtainAnInt32(const char *key)
{
	char res_decimals[15] = "";
	char *tail_res = res_decimals;
	uint8_t space_count = 0;
	uint8_t i = 0;
	do {
		if (isdigit(key[i]))
			strncat(tail_res++, &key[i], 1);
		if (key[i] == ' ')
			space_count++;
	} while (key[++i]);
	return ((uint32_t) strtoul(res_decimals, NULL, 10) / space_count);
}


/* TODO: Re-write this to suppor the different protocols!!! */
static int ws_get_handshake_answer_00(const struct ws_info *wsi,
		uint8_t *out_frame, size_t *out_len)
{
    unsigned int written = 0;
	assert(out_frame && *out_len && wsi);
	// wsi->key3 is always not NULL, for hybi protocol, key 2 is not set either
	if ( (wsi->host == NULL) || (wsi->key1 == NULL) )
	{
	    DEBUG_TRACE(("%s(): Invalid Websocket packet",__FUNCTION__));
	    return -1;
	}

    uint8_t chrkey1[4];
    uint8_t chrkey2[4];
    uint32_t key1 = doStuffToObtainAnInt32(wsi->key1);
    uint32_t key2 = doStuffToObtainAnInt32(wsi->key2);
    uint8_t i;
    for (i = 0; i < 4; i++)
        chrkey1[i] = key1 << (8 * i) >> (8 * 3);
    for (i = 0; i < 4; i++)
        chrkey2[i] = key2 << (8 * i) >> (8 * 3);

    uint8_t raw_md5[16];
    uint8_t keys[16];
    memcpy(keys, chrkey1, 4);
    memcpy(&keys[4], chrkey2, 4);
    memcpy(&keys[8], wsi->key3, 8);

    md5_state_t state;
    md5_init(&state);
    md5_append(&state, (const md5_byte_t *)keys, sizeof(keys) );
    md5_finish(&state, raw_md5 );

    written = sprintf_P((char *)out_frame, "%s\r\n%s: %s\r\n%s: %s\r\n%s: %s\r\n%s: ws://%s%s\r\n",
        ws_header_hixie_resp,
        ws_header_upgrade, ws_header_websocket,
        ws_header_connection, ws_header_upgrade,
        ws_header_sec_origin, wsi->origin,
        ws_header_sec_location, wsi->host, wsi->resource);

    if (wsi->protocol) {
        written += sprintf_P((char *)out_frame + written,
            PSTR("Sec-WebSocket-Protocol: %s\r\n"), wsi->protocol);
    }
    written += sprintf_P((char *)out_frame + written, rn);

    // if assert fail, that means, that we corrupt memory
    assert(written <= *out_len && written + sizeof (keys) <= *out_len);
    memcpy(out_frame + written, raw_md5, sizeof (keys));
    *out_len = written + sizeof (keys);

	return written;
}


static int ws_get_handshake_answer_08(const struct ws_info *wsi,
        char* out_frame, size_t *out_len)
{
    static const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    char accept_buf[128 + 37];
    unsigned char hash[20];
    int written = 0;
    int accept_len;

    strcpy(accept_buf, wsi->key1);
    strcpy(accept_buf + strlen(wsi->key1), GUID);

    SHA1((unsigned char *)accept_buf, strlen(wsi->key1) + strlen(GUID), hash);

    accept_len = lws_b64_encode_string((char *)hash, 20, accept_buf, sizeof (accept_buf));
    if (accept_len < 0)
    {
        fprintf(stderr, "Base64 encoded hash too long\n");
        goto bail;
    }

    /* create the response packet */

    written = sprintf(out_frame, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n",accept_buf);

    if (wsi->protocol) {
        strcat(out_frame,   "Sec-WebSocket-Protocol: ");
        strcat(out_frame, wsi->protocol);
    }

    /* TODO: Add Extension support */

    /* end of response packet */
    strcat(out_frame, "\r\n\r\n");

    written = strlen(out_frame);
    *out_len = written;

    return written;

bail:
    return -1;
}


static enum ws_frame_type ws_make_frame(struct ws_info *wsi, uint8_t *data, int data_len,
    int *out_len, enum ws_frame_type frame_type, uint8_t **pBuf)
{
    int frame_size = 0;
    int i = 0;
	assert(data);
	uint8_t *pOutput = NULL;
	
	if (frame_type != WS_TEXT_FRAME)
	{
	    ERR("Only support TEXT FRAMES for now");
	    return WS_ERROR_FRAME;
	}

	/* First need to figure out how much we have to grow the buffer by */
    if (wsi->version == hybi_00) {
        frame_size = 2;
    }
    else {
        if (data_len < 126) {
            frame_size = 2;
        }
        else if (data_len < 65536) {
            frame_size = 4;
        }
        else {
            frame_size = 7;
        }
    }

    pOutput = malloc(data_len+frame_size);
    if (pOutput == NULL)
    {
        ERR("Cannot create an output buffer!");
        return WS_ERROR_FRAME;
    }


    /* The framing is different for the differnt protocols */
	if (wsi->version == hybi_00) {
	    pOutput[i++] = '\x00';
	    memcpy(&pOutput[i++], data, data_len);
	    pOutput[data_len + 1] = '\xFF';
	}
	else {
	    pOutput[i++] = 0x81; /* TEXT FRAME */

	    if (data_len < 126) {
	        pOutput[i++] = data_len;
	    }
	    else if (data_len < 65536) {
	        pOutput[i++] =  126;
	        pOutput[i++] = data_len >> 8;
	        pOutput[i++] = data_len;
        }
	    else {
	        int j;
	        pOutput[i++] = 127;
            for (j = 5; j >= 0; j--, i++)
            {
                pOutput[i] = data_len >> (j * 8);
            }
	    }
	    memcpy(&pOutput[i], data, data_len);
	}

    *out_len = data_len + frame_size;
    *pBuf = pOutput;
	return frame_type;
}

static enum ws_frame_type ws_parse_input_frame(struct ws_info *wsi, const uint8_t *input_frame, size_t input_len,
		uint8_t **out_data_ptr, int *out_len, int *header_length)
{
	enum ws_frame_type frame_type;

	assert(out_len); 
	assert(input_len);

	if (input_len < 2)
		return WS_INCOMPLETE_FRAME;

	if ((input_frame[0]&0x80) != 0x80) // text frame
	{
		const uint8_t *data_start = &input_frame[1];
		uint8_t *end = (uint8_t *) memchr(data_start, 0xFF, input_len - 1);
		if (end) {
			assert((size_t)(end - data_start) <= input_len);
			*out_data_ptr = (uint8_t *)data_start;
			*out_len = end - data_start;
			frame_type = WS_TEXT_FRAME;
			DEBUG_TRACE(("%s(): Received WS_TEXT_FRAME", __FUNCTION__));
        *header_length = 2;
		} else {
			frame_type = WS_INCOMPLETE_FRAME;
		}
	} else if ((input_frame[0]&0x80) == 0x80) // binary frame
	{
		if (input_frame[0] == 0xFF && input_frame[1] == 0x00)
			frame_type = WS_CLOSING_FRAME;
		else {
			uint32_t data_length = 0;
			uint32_t old_data_length = 0;
			const uint8_t *frame_ptr = &input_frame[1];
			while ((*frame_ptr&0x80) == 0x80) {
				old_data_length = data_length;
				data_length *= 0x80;
				data_length += *frame_ptr & 0xF9;
				if (data_length < old_data_length || // overflow occured
						input_len < data_length) // something wrong
					return WS_ERROR_FRAME;
				frame_ptr++;
			}
			*out_data_ptr = (uint8_t *)frame_ptr;
			*out_len = data_length;
			DEBUG_TRACE(("%s(): Received WS_BINARY_FRAME", __FUNCTION__));
			frame_type = WS_BINARY_FRAME;
		}
	} else {
        frame_type = WS_ERROR_FRAME;
        DEBUG_TRACE(("%s(): Received WS_ERROR_FRAME", __FUNCTION__));
	}

	return frame_type;
}

/* The following routing will parse the header and if the http message was the
 * start of a WebSocket protocol (handshake) we complete the handshake and block
 * serving input from the socket.
 */
int mg_ws_parse_header(struct ws_info *wsi, int fd, struct mg_request_info *ri, char* buf, int length)
{
    int                 i = 0;
    char                *pKey3 = NULL;
    static char         buffer[BUF_LEN];
    size_t              out_len = BUF_LEN;
    int                 result = -1;
    int                 version = 0;
    int                 opt = 1;

    memset(wsi, 0, sizeof(struct ws_info));

    if ( (buf == NULL) || (ri == NULL))
    {
        ERR("Invalid parameter!");
        return -1;
    }

    /* First check if this is a valid Websocket message */
    for (i = 0; i < ri->num_headers; i++)
    {
        if (ri->http_headers[i].name == NULL)
            break;

        /* Connection */
        if ( (strcmp(ri->http_headers[i].name, ws_header_connection) == 0) &&
             (strcmp(ri->http_headers[i].value, ws_header_upgrade) != 0) )
        {
            DEBUG_TRACE(("%s(): Not a WebSocket Connection: ri->http_headers[i].name=%s, ws_header_connection=%s,strcmp(ri->http_headers[i].value=%s,  ws_header_upgrade=%s",
                __FUNCTION__, ri->http_headers[i].name, ws_header_connection, ri->http_headers[i].value, ws_header_upgrade));
            return 0;
        }

        else if ( (strcmp(ri->http_headers[i].name, ws_header_upgrade) == 0) &&
             (strcasecmp(ri->http_headers[i].value, ws_header_websocket) !=0) )
        {
            DEBUG_TRACE(("%s(): Not a WebSocket Upgrade: ri->http_headers[i].name=%s, ws_header_upgrade=%s, ri->http_headers[i].value=%s, ws_header_websocket=%s",
                __FUNCTION__, ri->http_headers[i].name, ws_header_upgrade, ri->http_headers[i].value, ws_header_websocket));
            return 0;
        }

        /* Origin */
        else if (strcmp(ri->http_headers[i].name, ws_header_host) == 0)
        {
            wsi->host = ri->http_headers[i].value;
        }

        /* Sec-WebSocket-Version */
        else if (strcmp(ri->http_headers[i].name, ws_header_sec_version) == 0)
        {
            version = atoi(ri->http_headers[i].value);
        }

        /* Sec-WebSocket-Key2 */
        else if (strcmp(ri->http_headers[i].name, ws_header_sec_key2) == 0)
        {
            wsi->key2 = ri->http_headers[i].value;
        }

        /* Host */
        else if (strcmp(ri->http_headers[i].name, ws_header_origin) == 0)
        {
            wsi->origin = ri->http_headers[i].value;
        }

        /* Sec-WebSocket-Protocol */
        else if (strcmp(ri->http_headers[i].name, ws_header_sec_protocol) == 0)
        {
            wsi->protocol = ri->http_headers[i].value;
        }

        /* Sec-WebSocket-Key1 or Sec-WebSocket-Key*/
        else if ( (strcmp(ri->http_headers[i].name, ws_header_sec_key1) == 0) ||
                  (strcmp(ri->http_headers[i].name, ws_header_sec_key) == 0) )
        {
            wsi->key1 = ri->http_headers[i].value;
        }

        /* Sec-WebSocket-Protocol */
        else if (strcmp(ri->http_headers[i].name, ws_header_sec_protocol) == 0)
        {
            wsi->protocol = ri->http_headers[i].value;
        }
    }

    wsi->resource = ri->uri;

    if (wsi->version == 0)
    {
        pKey3 = buf + length - 8;
        memcpy(wsi->key3, pKey3, sizeof(wsi->key3));
    }

    /* Formulate answer depending on protocol version */
    switch (version) {
    /* No Masking done */
    case 0:
        wsi->version = hybi_00;
        result = ws_get_handshake_answer_00(wsi, (uint8_t*)buffer, &out_len);
        break;

    case 5:
    case 6:
    case 7:
    case 8:
    case 13:
        wsi->version = hybi_08;
        result = ws_get_handshake_answer_08(wsi, buffer, &out_len);
        break;

    default:
        ERR("Unknown/Unsupported WebSocket Spec Version\n");
        break;
    }

    if (result <= 0)
        return result;

    if (send(fd, buffer, out_len, 0) < 0) {
        fprintf(stderr, "Send failed: %s\n", strerror(errno));
        return 0;
    }

    wsi->fd = fd;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    return result;
}

/*
 * The WebSocket protocol: draft-ietf-hybi-thewebsocketprotocol-08
 *
 *  0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-------+-+-------------+-------------------------------+
 *  |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 *  |I|S|S|S|  (4)  |A|     (7)     |             (16/63)           |
 *  |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 *  | |1|2|3|       |K|             |                               |
 *  +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 *  |     Extended payload length continued, if payload len == 127  |
 *  + - - - - - - - - - - - - - - - +-------------------------------+
 *  |                               |Masking-key, if MASK set to 1  |
 *  +-------------------------------+-------------------------------+
 *  | Masking-key (continued)       |          Payload Data         |
 *  +-------------------------------- - - - - - - - - - - - - - - - +
 *  :                     Payload Data continued ...                :
 *  + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *  |                     Payload Data continued ...                |
 *  +---------------------------------------------------------------+
 */
#define read_chunk(buf, length, next, result)                                   \
{                                                                               \
    int i;                                                                      \
    result = 0;                                                                 \
    /* Parse the length */                                                      \
    for (i = (length-1); i >= 0; i--, next++)                                   \
    {                                                                           \
        result |= (buf[next] << i*8);                                           \
    }                                                                           \
}

unsigned char xor_mask(uint8_t *masking_key, int key_count, unsigned char c)
{
    return c ^ masking_key[key_count & 3];
}


static enum ws_frame_type ws_parse_input_frame_08(struct ws_info *wsi, const uint8_t *input_frame, size_t input_len,
        uint8_t **out_data_ptr, int *out_len, int *header_length) {

    enum ws_frame_type  frame_type = WS_ERROR_FRAME;
    uint32_t        packet_length = 0;
    int             current = 0;
    char            frame_masked;
    uint8_t         masking_key[4] = {0,0,0,0};

    switch (input_frame[0] & 0x0f)
    {
    case 0x01:
        DEBUG_TRACE(("%s(): Received WS_TEXT_FRAME", __FUNCTION__));
        frame_type = WS_TEXT_FRAME;
        break;
    case 0x02:
        DEBUG_TRACE(("%s(): Received WS_BINARY_FRAME", __FUNCTION__));
        frame_type = WS_BINARY_FRAME;
        break;
    case 0x08:
        DEBUG_TRACE(("%s(): Received WS_CLOSING_FRAME", __FUNCTION__));
        frame_type = WS_CLOSING_FRAME;
        break;
    default:
        DEBUG_TRACE(("%s(): Received WS_ERROR_FRAME", __FUNCTION__));
        return WS_ERROR_FRAME;
    }

    frame_masked = (input_frame[1] & 0x80);

    current = 2;
    /* Payload Length */
    switch (input_frame[1] & 0x7f)
    {
    case 126:
        read_chunk(input_frame, 2, current, packet_length);
        break;
    case 127:
        read_chunk(input_frame, 8, current, packet_length);
        break;
    default:
        packet_length = input_frame[1] & 0x7f;
        break;
    }
    if (packet_length > input_len)
    	return WS_INCOMPLETE_FRAME;
    if (frame_masked)
    {
        int i = 0;
        for (i = 0; i < 4; i++, current++)
            masking_key[i] = input_frame[current];
    }

    *out_data_ptr = (uint8_t*)input_frame + current;
    *out_len = (int)packet_length;
    *header_length = current;

    /* Now we have the payload so lets XOR it (or not) */
    if (masking_key[0] || masking_key[2] || masking_key[2] || masking_key[3])
    {
        int count;
        int masking_key_count = 0;
        uint8_t *c = (uint8_t*)&input_frame[current];

        for (count = current; count < (packet_length+current); count++, c++, masking_key_count++)
        {
            *c = xor_mask(masking_key, masking_key_count, input_frame[count]);
			if (frame_type == WS_TEXT_FRAME && count < (packet_length + current - 1) && !isprint(*c)) {
				fprintf(stderr, "%s(): Received invalid character %c (0x%x) at position %d in WS_TEXT_FRAME.\n",
						__FUNCTION__, *c, *c, count - current);
				if (count - current >= packet_length - 2) /* last byte or two may be read only half way? */
					return WS_INCOMPLETE_FRAME;
				return WS_ERROR_FRAME;
			}
        }

		if (frame_type == WS_TEXT_FRAME && count > 0 && input_frame[current] == '{' && input_frame[count - 1] != '}') {
			DEBUG_TRACE(("%s(): json string doesn't end with }. Repaired it.\n", __FUNCTION__));
			*(c - 1) = '}';
		}
    }

    return frame_type;
}


int mg_ws_receive(struct ws_info *wsi, char *buf, int read_length, char** in_data, int *data_length, enum ws_frame_type *type) {

    enum ws_frame_type  frame_type = WS_INCOMPLETE_FRAME;
    int header_length = 0;

    if ( (wsi == NULL) || (buf == NULL) || (in_data == NULL) || (data_length == NULL) )
    {
        ERR("Passed garbage");
        return -1;
    }

    if (wsi->version == hybi_00) {
        frame_type = ws_parse_input_frame(wsi, (const uint8_t*)buf, read_length, (uint8_t**)in_data, data_length, &header_length);
    }
    else
    {
        frame_type = ws_parse_input_frame_08(wsi, (const uint8_t*)buf, read_length, (uint8_t**)in_data, data_length, &header_length);
    }

    if (type)
    	*type = frame_type;

    switch(frame_type)
    {
    case WS_INCOMPLETE_FRAME:
        return -2;

    case WS_CLOSING_FRAME:
        send(wsi->fd, "\xFF\x00", 2, 0); // send closing frame
        return 0; /* we are done so exit */
        break;

    case WS_TEXT_FRAME:
        return (header_length + *data_length);
        break;

    case WS_ERROR_FRAME:
    default:
        ERR("Error in the WS Frame");
        return -1;
    }

    return 0;
}


int mg_ws_write(struct ws_info *wsi, const char *data, int data_len) {

    int new_length = 0;
    int32_t sent = 0;
    int n, k;
    uint8_t *pNewBuf = NULL;

    if (ws_make_frame(wsi, (uint8_t*)data, data_len, &new_length, WS_TEXT_FRAME, &pNewBuf) != WS_TEXT_FRAME) {
        fprintf(stderr, "Make frame failed\n");
        return -1;
    }

    if (pNewBuf != NULL) {
        while (sent < new_length) {
            /* How many bytes we send in this iteration */
            k = new_length - sent > INT_MAX ? INT_MAX : (int) (new_length - sent);
            n = send(wsi->fd, pNewBuf + sent, (size_t)k, 0);

            if (n < 0)
              break;

            sent += n;
        }
        free (pNewBuf);
    }
    return sent;
}
