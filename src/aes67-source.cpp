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

#include "plugin-macros.generated.h"

struct aes67_source
{
	obs_source_t* source;
};

const char* aes67_source_getname(void* data)
{
	UNUSED_PARAMETER(data);
	return "AES67 Source";
}

obs_properties_t* aes67_source_getproperties(void* data)
{
	auto s = (struct aes67_source*)data;

	obs_properties_t* props = obs_properties_create();
	obs_properties_set_flags(props, OBS_PROPERTIES_DEFER_UPDATE);

	return props;
}

void aes67_source_getdefaults(obs_data_t* settings)
{
	// TODO
	UNUSED_PARAMETER(settings);
}

void aes67_source_update(void* data, obs_data_t* settings)
{
	auto s = (struct aes67_source*)data;
	// TODO
	UNUSED_PARAMETER(s);
}

void* aes67_source_create(obs_data_t* settings, obs_source_t* source)
{
	auto s = (struct aes67_source*)bzalloc(sizeof(struct aes67_source));
	s->source = source;
	aes67_source_update(s, settings);
	return s;
}

void aes67_source_destroy(void* data)
{
	auto s = (struct aes67_source*)data;
	bfree(s);
}

const struct obs_source_info create_aes67_source_info()
{
	struct obs_source_info source_info = {};

	source_info.id				= "aes67_source";
	source_info.type			= OBS_SOURCE_TYPE_INPUT;
	source_info.output_flags	= OBS_SOURCE_AUDIO | OBS_SOURCE_DO_NOT_DUPLICATE;

	source_info.get_name		= aes67_source_getname;
	source_info.get_properties	= aes67_source_getproperties;
	source_info.get_defaults	= aes67_source_getdefaults;

	source_info.update			= aes67_source_update;
	source_info.create			= aes67_source_create;
	source_info.destroy			= aes67_source_destroy;

	return source_info;
}