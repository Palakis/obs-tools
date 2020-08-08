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

#pragma once

#include <stdint.h>

typedef struct {
	uint8_t version;
	bool padding;
	bool extension;
	uint8_t csrcCount;
	bool marker;
	uint8_t payloadType;
	uint16_t sequenceNumber;
	uint32_t timestamp;
	uint32_t ssrc;
	uint32_t _csrcFieldLength;
	uint32_t* csrc;
	uint16_t extensionHeaderId;
	uint16_t extensionHeaderLength;
	uint8_t* extensionHeader;
	uint32_t payloadLength;
	uint8_t* payload;
} rtp_packet;

rtp_packet* rtp_packet_new();
void rtp_packet_free(rtp_packet* packet);
bool rtp_packet_decode(const uint8_t* buf, size_t bufLen, rtp_packet* packet);
size_t rtp_packet_encode(const rtp_packet* packet, uint8_t* buf, size_t bufLen);
size_t rtp_packet_get_byte_count(const rtp_packet* packet);
