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

#include <util/bmem.h>

#include "plugin-macros.generated.h"
#include "rtp.h"
#include "utils.h"

using namespace std;
using namespace utils;

bool rtp_packet_decode(const uint8_t* buf, const uint32_t bufLen, rtp_packet* packet)
{
	if (!packet) {
		blog(LOG_ERROR, "rtp_packet_decode: invalid dest packet pointer");
		return false;
	}

	if (bufLen <= 12) {
		blog(LOG_ERROR, "rtp_packet_decode: packet too short (%d bytes instead of > 12)", bufLen);
		return false;
	}

	// First byte
	uint8_t rtp_version = ((buf[0] >> 6) & 0b11);
	if (rtp_version != 2) {
		blog(LOG_ERROR, "rtp_packet_decode: invalid packet version: %d", rtp_version);
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

	size_t expectedBufferLength;

	if (packet->csrcCount > 0) {
		if (packet->csrcCount > 15) {
			blog(LOG_ERROR, "rtp_packet_decode: invalid CSRC count (found %d, should not be above 15)", packet->csrcCount);
			return false;
		}
		
		size_t csrcFieldLength = (packet->csrcCount * 4);
		expectedBufferLength = index + csrcFieldLength;
		if (expectedBufferLength > bufLen) {
			blog(LOG_ERROR, "rtp_packet_decode: buffer too short for specified CSRC count (%d bytes < %d)", bufLen, expectedBufferLength);
			return false;
		}

		for (size_t i = 0; i < packet->csrcCount; i++) {
			const uint8_t* valuePtr = &buf[index + (i * sizeof(uint32_t))];
			packet->csrc[i] = read_uint32(valuePtr);
		}

		index += csrcFieldLength;
	}

	if (packet->extension) {
		expectedBufferLength = (index + 4);
		if (expectedBufferLength > bufLen) {
			blog(LOG_ERROR, "rtp_packet_decode: buffer too short for extension header (%d bytes < %d)", bufLen, expectedBufferLength);
			return false;
		}

		packet->extensionHeaderId = read_uint16(&buf[index]);
		packet->extensionHeaderLength = read_uint16(&buf[index + 2]);
		index += 4;

		expectedBufferLength = (index + packet->extensionHeaderLength);
		if (expectedBufferLength > bufLen) {
			blog(LOG_ERROR, "rtp_packet_decode: buffer too short for specified extension data length (%d bytes < %d)", bufLen, expectedBufferLength);
			return false;
		}

		memcpy(
			&packet->extensionHeader, &buf[index],
			packet->extensionHeaderLength
		);

		index += packet->extensionHeaderLength;
	}

	packet->payloadLength = (bufLen - index);	
	memcpy(
		&packet->payload, &buf[index],
		packet->payloadLength
	);

	// TODO return read bytes
	return true;
}

size_t rtp_packet_get_byte_count(const rtp_packet* packet)
{
	if (!packet) {
		return 0;
	}

	// minimal size: 12 bytes + 4 bytes per CSRC + payload length
	size_t packetSize = 12 + (packet->csrcCount * sizeof(uint32_t)) + packet->payloadLength;

    // if extension is set, add 4 bytes of extension preamble + extension header byte count
	if (packet->extension) {
		packetSize += (4 + packet->extensionHeaderLength);
	}

	return packetSize;
}

