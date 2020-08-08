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

#include "utils.h"

void utils::register_frontend_event_once(enum obs_frontend_event cbEventCode, event_cb cb, void* private_data)
{
	typedef struct {
		enum obs_frontend_event eventCode;
		event_cb nestedCallback;
		void* nestedCallbackParam;

		obs_frontend_event_cb eventHandler;
	} cb_context;

	cb_context* ctx = reinterpret_cast<cb_context*>(
		bmalloc(sizeof(cb_context))
	);
	ctx->eventCode = cbEventCode;
	ctx->nestedCallback = cb;
	ctx->nestedCallbackParam = private_data;

	ctx->eventHandler = [](enum obs_frontend_event localEventCode, void* private_data) {
		cb_context* ctx = reinterpret_cast<cb_context*>(private_data);

		if (localEventCode == ctx->eventCode) {
			ctx->nestedCallback(ctx->nestedCallbackParam);
			obs_frontend_remove_event_callback(ctx->eventHandler, private_data);
			bfree(private_data);
		}
	};

	obs_frontend_add_event_callback(ctx->eventHandler, (void*)ctx);
}

uint16_t utils::read_uint16(const uint8_t* src)
{
	return (uint16_t)(
		((src[0] << 8) & 0xFF00) |
		( src[1]       & 0x00FF)
	);
}

void utils::write_uint16(uint8_t* dest, const uint16_t value)
{
	dest[0] = (value & 0xFF00) >> 8;
	dest[1] = (value & 0x00FF);
}

uint32_t utils::read_uint32(const uint8_t* src)
{
	return (uint32_t)(
		((src[0] << 24) & 0xFF000000) |
		((src[1] << 16) & 0x00FF0000) |
		((src[2] << 8)  & 0x0000FF00) |
		( src[3]        & 0x000000FF)
	);
}

void utils::write_uint32(uint8_t* dest, const uint32_t value)
{
	dest[0] = (value & 0xFF000000) >> 24;
	dest[1] = (value & 0x00FF0000) >> 16;
	dest[2] = (value & 0x0000FF00) >> 8;
	dest[3] = (value & 0x000000FF);
}
