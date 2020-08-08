/*
obs-tools
Copyright (C) 2020	Stéphane Lepin <stephane.lepin@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <memory>
#include <cstring>
#include <iostream>
#include <util/bmem.h>

#include "rtp.h"
#include "utils.h"

using namespace std;
using namespace utils;

void rtp_packet_deinit(rtp_packet* packet)
{
	if (!packet) {
		return;
	}

	if (packet->csrcCount > 0) {
		bfree(packet->csrc);
	}

	if (packet->extension) {
		bfree(packet->extensionHeader);
	}

	if (packet->payload) {
		bfree(packet->payload);
	}
}

void* safe_brealloc(void* ptr, size_t size)
{
	if (ptr) {
		return brealloc(ptr, size);
	} else {
		return bmalloc(size);
	}
}

bool rtp_packet_decode(const uint8_t* buf, uint32_t bufLen, rtp_packet* packet)
{
	if (!packet) {
		cerr << "rtp_packet_decode: invalid dest packet pointer" << endl;
		return false;
	}

	if (bufLen <= 12) {
		cerr << "rtp_packet_decode: packet too short (" << bufLen << "bytes)" << endl;
		return false;
	}

	// First byte
	uint8_t rtp_version = ((buf[0] >> 6) & 0b11);
	if (rtp_version != 2) {
		cout << "rtp_packet_decode: invalid packet version: " << packet->version << endl;
		return false;
	}
	packet->version = rtp_version;

	uint32_t index = 0;

	// First byte
	packet->padding = ((buf[index] >> 5) & 0b1);
	packet->extension = ((buf[index] >> 4) & 0b1);
	packet->csrcCount = (buf[index] & 0x0F);
	index++;

	// Second byte
	packet->marker = ((buf[index] >> 7) & 0b1);
	packet->payloadType = (buf[index] & 0x7F);
	index++;

	// Third & fourth byte
	packet->sequenceNumber = read_uint16(&buf[index]);
	index += 2;

	// Timestamp from four bytes
	packet->timestamp = read_uint32(&buf[index]);
	index += 4;

	// SSRC from four bytes
	packet->ssrc = read_uint32(&buf[index]);
	index += 4;

	if (packet->csrcCount > 0) {
		if (packet->csrcCount > 15) {
			cerr << "rtp_packet_decode: invalid CSRC count"
				 << " (found " << packet->csrcCount << ", should not be above 15)" << endl;
		 }

		uint32_t csrcFieldLength = (packet->csrcCount * 4);
		if (csrcFieldLength != packet->_csrcFieldLength) {
			packet->csrc = (uint32_t*)safe_brealloc(packet->csrc, csrcFieldLength);
		}
		packet->_csrcFieldLength = csrcFieldLength;

		for (size_t i = 0; i < packet->csrcCount; i++) {
			const uint8_t* valuePtr = &buf[index + (i * sizeof(uint32_t))];
			packet->csrc[i] = read_uint32(valuePtr);
		}

		index += csrcFieldLength;
	}

	if (packet->extension) {
		packet->extensionHeaderId = read_uint16(&buf[index]);
		uint16_t extensionHeaderLength = read_uint16(&buf[index + 2]);
		index += 4;

		if (extensionHeaderLength != packet->extensionHeaderLength) {
			packet->extensionHeader = (uint8_t*)safe_brealloc(packet->extensionHeader, extensionHeaderLength);
		}
		packet->extensionHeaderLength = extensionHeaderLength;

		memcpy(
			packet->extensionHeader, &buf[index],
			packet->extensionHeaderLength
		);

		index += packet->extensionHeaderLength;
	}

	uint32_t payloadLength = (bufLen - index);
	if (payloadLength != packet->payloadLength) {
		packet->payload = (uint8_t*)safe_brealloc(packet->payload, payloadLength);
	}
	packet->payloadLength = payloadLength;
	
	memcpy(
		packet->payload, &buf[index],
		packet->payloadLength
	);

	// TODO return read bytes
	return true;
}
