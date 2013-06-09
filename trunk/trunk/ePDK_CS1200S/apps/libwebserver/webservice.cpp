/*
 * webservice.cpp
 *
 *  Created on: Jun 8, 2013
 *      Author: buttonfly
 */

#include "webservice.h"

#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include <assert.h>
#include <string>
#include <iostream>

#include "osldbg.h"

using namespace std;


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

static int websocketConnect(const struct mg_connection *conn) {

	S_;
	return 0;
}

static int  log(const struct mg_connection *conn, const char *message) {
	S_;
	fprintf(stderr, "%s\n", message);
	return 0;
}

static int beginRequest(struct mg_connection *conn) {
	S_;
#if fixme
  const struct mg_request_info *request_info = mg_get_request_info(conn);
  char content[100];

  // Prepare the message we're going to send
  int content_length = snprintf(content, sizeof(content),
                                "Hello from mongoose! Remote port: %d",
                                request_info->remote_port);

  // Send HTTP reply to the client
  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"        // Always set Content-Length
            "\r\n"
            "%s",
            content_length, content);
  // Returning non-zero tells mongoose that our function has replied to
   // the client, and mongoose should not send client any more data.
   return 0;
#endif
   const struct mg_request_info *request_info = mg_get_request_info(conn);
//   M_("%s", request_info->http_headers);
//   M_("%s", request_info->query_string);
//   M_("%s", request_info->request_method);
   return 0;
}

static void websocketReady(struct mg_connection *conn) {
	S_;
	unsigned char buf[40];
	buf[0] = 0x81;
	buf[1] = snprintf((char *) buf + 2, sizeof(buf) - 2, "%s", "server ready");
	mg_write(conn, buf, 2 + buf[1]);
}

#define WEBSOCKET_TEXT_FRAME				0x01
#define WEBSOCKET_BINARY_FRAME			0x02
#define WEBSOCKET_CLOSING_FRAME		0x08
#define WEBSOCKET_ERROR_FRAME			0x00

static int websocketRead(struct mg_connection* conn) {


}

static int websocketWrite(struct mg_connection* conn) {

	return 0;
}

static int websocketData(struct mg_connection *conn) {
	S_;
#if fixme
	unsigned char buf[200], reply[200];
	int n, i, mask_len, x, msg_len, len;

	// Read message from the client.
	// Accept only small (<126 bytes) messages.
	len = 0;
	msg_len = mask_len = 0;
	for (;;) {
		if ((n = mg_read(conn, buf + len, sizeof(buf) - len)) <= 0) {
			return 0;  // Read error, close websocket
		}
		len += n;
		if (len >= 2) {
			msg_len = buf[1] & 127;
			mask_len = (buf[1] & 128) ? 4 : 0;
			if (msg_len > 125) {
				fprintf(stderr, "W:message is too long. We are going to close this websocket.\n");
				return 0; // Message is too long, close websocket
			}
			// If we've buffered the whole message, exit the loop
			if (len >= 2 + mask_len + msg_len) {
				break;
			}
		}
	}

	// TODO
	// find here
	// data received

	struct mg_request_info* requestInfo = mg_get_request_info(conn);
	k::WebService* context = static_cast<k::WebService*>(requestInfo->user_data);
	assert(context);
	string request = string((char*)buf, len);
	cerr << request << endl;
	string response = context->process(request);
	cerr << response << endl;
	// write
	// Prepare frame
	reply[0] = 0x81;  // text, FIN set
	reply[1] = msg_len;

	// Copy message from request to reply, applying the mask if required.
	for (i = 0; i < msg_len; i++) {
		x = mask_len == 0 ? 0 : buf[2 + (i % 4)];
		reply[i + 2] = buf[i + 2 + mask_len] ^ x;
	}

	// Echo the message back to the client
	mg_write(conn, reply, 2 + msg_len);

	// Returnint zero means stoping websocket conversation.
	// Close the conversation if client has sent us "exit" string.
	return memcmp(reply + 2, "exit", 4);
#endif
	int i, mask_len, x, msg_len;
	uint8_t buf[10];

	msg_len = mask_len = 0;
	for (int len=0, n=0; len < 2; len += n) {
		if ((n = mg_read(conn, buf + len, 2 - len)) <= 0)
			return 0;  // Read error, close websocket
	}

	if(buf[0] != 0x81)
		return 0;

	char frame_masked = (buf[1] & 0x80);
	if(frame_masked) {
//		M_("frame masked");
	}

	mask_len = (buf[1] & 128) ? 4 : 0;
	int current = 2;
	switch(buf[1] & 0x7F) {
	case 126: {
		for (int len=0, n=0; len < 2; len+=n) {
			if((n = mg_read(conn, buf+len, 2-len)) <=0)
				return 0;
		}
		msg_len = (((int) buf[0] << 8) + buf[1]);
		break;
	}
	case 127: {
		for (int len=0, n=0; len < 2; len += n) {
			if((n = mg_read(conn, buf+len, 2-len)) <=0)
				return 0;
		}
		msg_len = (((long long) htonl(*(int*)&buf[0])) << 32) | htonl(*(int*)&buf[4]);
		break;
	}
	default:
		//TODO:
		fprintf(stderr, "unsupported length: %d\n", msg_len);
		return 0;
	}

	uint8_t masking_key[4] = {0,0,0,0};
    if (frame_masked){
        int i = 0;
        for (i = 0; i < 4; i++, current++)
            masking_key[i] = buf[current];
    }

//	M_("msg len: %d", msg_len);
	uint8_t* data = new uint8_t[msg_len+mask_len];
	for(int len=0, n=0;len < msg_len+mask_len; len += n) {
		if ((n = mg_read(conn, data+len, msg_len+mask_len-len)) <= 0) {
			fprintf(stderr, "error\n");
			delete[] data;
			return 0;  // Read error, close websocket
		}
	}

	char* msg = new char[msg_len+1];
	for(int i=0; i < msg_len; i++) {
		int x = ((mask_len==0) ? 0 : data[i%4]);
		msg[i] = data[i+mask_len]^x;
	}
	msg[msg_len] = '\0';

	// TODO : hook here.


	delete[] data;
	M_("%s", msg);
	delete[] msg;
	return 1;
}

namespace k {

WebService::WebService() :
		m_context(NULL) {
}

WebService::~WebService() {
}

int WebService::start(void) {

	struct mg_callbacks callbacks;
	const char *options[] = { "listening_ports", "8080", "document_root",
			"webapp", NULL };

	memset(&callbacks, 0, sizeof(callbacks));

//	callbacks.websocket_ready = websocketReady;
	callbacks.websocket_data = websocketData;
	callbacks.websocket_connect = websocketConnect;
	callbacks.log_message = log;
//	callbacks.begin_request = beginRequest;

	m_context = mg_start(&callbacks, this, options);
	return 0;
}

int WebService::stop(void) {
	mg_stop((struct mg_context*) m_context);
	return 0;
}

std::string WebService::process(std::string& data) {
	S_;
	cerr << data << endl;
	return "";
}

int WebService::add(IPlugin* p) {
	p->bind(m_jsonHandler);
	return 0;
}

int WebService::remove(IPlugin* p) {
	p->unbind(m_jsonHandler);
	return 0;
}


} /* namespace k */
