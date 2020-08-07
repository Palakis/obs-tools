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


#include <obs-module.h>
#include <obs-frontend-api.h>

#include "plugin-macros.generated.h"

#include "utils.h"
#include "status-label.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

void on_finished_loading(void* param);

bool obs_module_load(void)
{
	blog(LOG_INFO, "version %s", PLUGIN_VERSION);

	// defer GUI setup when frontend is ready
	utils::register_frontend_event_once(
		OBS_FRONTEND_EVENT_FINISHED_LOADING, on_finished_loading, nullptr
	);

	blog(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	return true;
}

void on_finished_loading(void* param)
{
	// call setup procedure(s)
	status_label::setup();

	UNUSED_PARAMETER(param);
}

void obs_module_unload()
{
	// call teardown procedure(s)
	status_label::teardown();

	blog(LOG_INFO, "plugin unloaded");
}
