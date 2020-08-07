/*
obs-tools
Copyright (C) 2020	St�phane Lepin <stephane.lepin@gmail.com>

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

#include "status-label.h"

#include <obs-frontend-api.h>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLayout>

#include "QStatusLabel.h"

QStatusLabel* streamStatus;
QStatusLabel* recordingStatus;

void on_streaming_status_changed(enum obs_frontend_event event, void* param);

void status_label::setup()
{
	QMainWindow* mainWindow = reinterpret_cast<QMainWindow*>(
		obs_frontend_get_main_window()
	);

	streamStatus = new QStatusLabel("OFF AIR", "ON AIR");
	recordingStatus = new QStatusLabel("REC OFF", "REC ON");

	QWidget* controlsDockContents = mainWindow->findChild<QWidget*>("controlsDockContents");
	if (controlsDockContents) {
		// create horizontal layout, with stream status on the left and recording status
		QHBoxLayout* hboxLayout = new QHBoxLayout();
		hboxLayout->setContentsMargins(0, 0, 0, 0);
		hboxLayout->addWidget(streamStatus);
		hboxLayout->addWidget(recordingStatus);

		QWidget* statusRow = new QWidget();
		statusRow->setMinimumHeight(50);
		statusRow->setLayout(hboxLayout);

		// add status row to controls section
		controlsDockContents->layout()->addWidget(statusRow);

		// register stream status changes callback
		obs_frontend_add_event_callback(on_streaming_status_changed, nullptr);
	}
}

void status_label::teardown()
{
	// no need to call delete or deleteLater on statusLabel
	obs_frontend_remove_event_callback(on_streaming_status_changed, nullptr);
}

void on_streaming_status_changed(enum obs_frontend_event event, void* param)
{
	if (streamStatus) {
		switch (event) {
		case OBS_FRONTEND_EVENT_STREAMING_STARTED:
			streamStatus->turnOn();
			break;
		case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
			streamStatus->turnOff();
			break;
		}
	}

	if (recordingStatus) {
		switch (event) {
		case OBS_FRONTEND_EVENT_RECORDING_STARTED:
			recordingStatus->turnOn();
			break;
		case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
			recordingStatus->turnOff();
			break;
		}
	}
}
