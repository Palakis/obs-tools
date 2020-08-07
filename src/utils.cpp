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
