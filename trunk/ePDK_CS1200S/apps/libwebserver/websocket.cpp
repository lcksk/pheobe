/*
 * websocket.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: buttonfly
 */

#include "websocket.h"

#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include <assert.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <arpa/inet.h>


namespace k {

WebSocket::WebSocket(struct mg_connection* conn)
: m_conn(conn)
{}

WebSocket::~WebSocket() {
	// TODO Auto-generated destructor stub
}

std::string WebSocket::read() {
	int i, mask_len, x, msg_len;
	uint8_t buf[10];

	msg_len = mask_len = 0;
	for (int len=0, n=0; len < 2; len += n) {
		if ((n = mg_read((struct mg_connection*) m_conn, buf + len, 2 - len)) <= 0)
			return "";  // Read error, close websocket
	}

	if(buf[0] != 0x81)
		return "";

	char frame_masked = (buf[1] & 0x80);
	if(frame_masked) {
//		M_("frame masked");
	}

	msg_len = buf[1] & 0x7F;
	mask_len = (buf[1] & 128) ? 4 : 0;
	int current = 2;

//	M_("msg len, mask len: %d, %d", msg_len, mask_len);
	if(msg_len == 126) { // large
		for (int len=0, n=0; len < 2; len+=n) {
			if((n = mg_read((struct mg_connection*) m_conn, buf+len, 2-len)) <=0)
				return "";
		}
		msg_len = (((int) buf[0] << 8) + buf[1]);
	}
	else if(msg_len > 126) { // very large
		for (int len=0, n=0; len < 2; len += n) {
			if((n = mg_read((struct mg_connection*) m_conn, buf+len, 2-len)) <=0)
				return "";
		}
		msg_len = (((long long) htonl(*(int*)&buf[0])) << 32) | htonl(*(int*)&buf[4]);
	}

	uint8_t masking_key[4] = {0,0,0,0};
    if (frame_masked){
        int i = 0;
        for (i = 0; i < 4; i++, current++)
            masking_key[i] = buf[current];
    }

	uint8_t* data = new uint8_t[msg_len+mask_len];
	for(int len=0, n=0;len < msg_len+mask_len; len += n) {
		if ((n = mg_read((struct mg_connection*) m_conn, data+len, msg_len+mask_len-len)) <= 0) {
			fprintf(stderr, "error\n");
			delete[] data;
			return "";  // Read error, close websocket
		}
	}

	char* msg = new char[msg_len+1];
	for(int i=0; i < msg_len; i++) {
		int x = ((mask_len==0) ? 0 : data[i%4]);
		msg[i] = data[i+mask_len]^x;
	}
	msg[msg_len] = '\0';
	std::string tmp(msg);
	delete[] data;
	delete[] msg;
	return tmp;
}

int WebSocket::write(std::string& msg) {
	// TODO
	int len = msg.length();
	if(len<=0)
		return -1;

	unsigned char* buf = new unsigned char[len+1];
	buf[0] = 0x81;  // text, FIN set
	buf[1] = len & 0x7F;

	for (int i = 0; i < len; i++) {
		buf[i + 2] = msg.c_str()[i];
	}

	len = mg_write((struct mg_connection*) m_conn, buf, 2 + len);

	delete[] buf;
	return (memcmp(buf + 2, "exit", 4)==0) ? 0 : len;
}


} /* namespace k */
