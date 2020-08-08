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

#include <obs.h>
#include <obs-module.h>
#include <util/platform.h>
#include <util/threading.h>

#include "plugin-macros.generated.h"

#define P_MULTICAST_GROUP "multicast_group"
#define P_SAMPLE_RATE "sample_rate"
#define P_SPEAKER_LAYOUT "speaker_layout"
#define P_SAMPLE_FORMAT "sample_format"

#define O_SAMPLE_FORMAT_L16 1
#define O_SAMPLE_FORMAT_L24 2

struct aes67_source
{
	obs_source_t* source;
	bool running;
	pthread_t receiver_thread;
	const char* multicast_group;
	int sample_rate;
	enum speaker_layout speakers;
	int sample_format;
	os_performance_token_t* perf_token;
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

	obs_properties_add_text(props,
		P_MULTICAST_GROUP, "Multicast Group",
		OBS_TEXT_DEFAULT
	);

	obs_property_t* prop;
	prop = obs_properties_add_list(props,
		P_SAMPLE_RATE, "Sample Rate",
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT
	);
	obs_property_list_add_int(prop, "48 kHz", 48000);
	obs_property_list_add_int(prop, "96 kHz", 96000);

	prop = obs_properties_add_list(props,
		P_SPEAKER_LAYOUT, "Channels",
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT
	);
	obs_property_list_add_int(prop, "Mono", SPEAKERS_MONO);
	obs_property_list_add_int(prop, "Stereo", SPEAKERS_STEREO);
	obs_property_list_add_int(prop, "7.1", SPEAKERS_7POINT1);

	prop = obs_properties_add_list(props,
		P_SAMPLE_FORMAT, "Sample Format",
		OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT
	);
	obs_property_list_add_int(prop, "L16", O_SAMPLE_FORMAT_L16);
	obs_property_list_add_int(prop, "L24", O_SAMPLE_FORMAT_L24);

	return props;
}

void aes67_source_getdefaults(obs_data_t* settings)
{
	obs_data_set_default_string(settings, P_MULTICAST_GROUP, "");
	obs_data_set_default_int(settings, P_SAMPLE_RATE, 48000);
	obs_data_set_default_int(settings, P_SPEAKER_LAYOUT, (int)SPEAKERS_STEREO);
	obs_data_set_default_int(settings, P_SAMPLE_FORMAT, O_SAMPLE_FORMAT_L24);
}

void* aes67_receiver_thread(void* data)
{
	auto s = (struct aes67_source*)data;
	
	blog(LOG_INFO, "receiver thread started for source '%s'", obs_source_get_name(s->source));

	if (s->perf_token) {
		os_end_high_performance(s->perf_token);
	}
	s->perf_token = os_request_high_performance("AES67 Receiver Thread");

	while (s->running) {
		// TODO loop
		os_sleep_ms(1);
	}

	os_end_high_performance(s->perf_token);
	s->perf_token = NULL;

	blog(LOG_INFO, "receiver thread ended for source '%s'", obs_source_get_name(s->source));
	return NULL;
}

void aes67_source_update(void* data, obs_data_t* settings)
{
	auto s = (struct aes67_source*)data;
	
	if (s->running) {
		s->running = false;
		pthread_join(s->receiver_thread, NULL);	
	}
	s->running = false;

	s->multicast_group = obs_data_get_string(settings, P_MULTICAST_GROUP);
	s->sample_rate = obs_data_get_int(settings, P_SAMPLE_RATE);
	s->speakers = (enum speaker_layout)obs_data_get_int(settings, P_SPEAKER_LAYOUT);
	s->sample_format = obs_data_get_int(settings, P_SAMPLE_FORMAT);

	// TODO setup receiver

	s->running = true;
	pthread_create(&s->receiver_thread, NULL, &aes67_receiver_thread, (void*)s);
}

void* aes67_source_create(obs_data_t* settings, obs_source_t* source)
{
	auto s = (struct aes67_source*)bzalloc(sizeof(struct aes67_source));
	s->source = source;
	s->perf_token = NULL;
	aes67_source_update(s, settings);
	return s;
}

void aes67_source_destroy(void* data)
{
	auto s = (struct aes67_source*)data;
	s->running = false;
	pthread_join(s->receiver_thread, NULL);
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