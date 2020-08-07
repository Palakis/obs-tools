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

#include "plugin-macros.generated.h"

#include "QStatusLabel.h"

#include <QtCore/QSize>
#include <QResizeEvent>

QStatusLabel::QStatusLabel(const QString& offAirText, const QString& onAirText, QWidget* parent) :
	QLabel(parent),
	_offText(offAirText),
	_onText(onAirText),
	_textSize("14px")
{
	setTextFormat(Qt::RichText);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	// call it on init to setup the stylesheet
	turnOff();
}

void QStatusLabel::setStatus(const QString& text, const QColor& textColor, const QColor& bgColor)
{
	_textColor = textColor;
	_bgColor = bgColor;
	refreshStyleSheet();

	setText(text);
}

void QStatusLabel::turnOn()
{
	setStatus(_onText, Qt::white, Qt::red);
}

void QStatusLabel::turnOff()
{
	setStatus(_offText, Qt::black, Qt::gray);
}

void QStatusLabel::resizeEvent(QResizeEvent* ev)
{
	_textSize = computeTextSize(ev->size());
	refreshStyleSheet();
}

const QString QStatusLabel::computeTextSize(const QSize& size)
{
	uint sizePx = (size.height() / 2);
	return QString("%1px").arg(sizePx);
}

void QStatusLabel::refreshStyleSheet()
{
	const QString stylesheetTemplate =
		"QStatusLabel {"
		"  font-weight: bold;"
		"  font-size: %1;"
		"  border: 1px solid %2;"
		"  color: %2;"
		"  background-color: %3;"
		"}";

	QString textColorHex = _textColor.name();
	QString bgColorHex = _bgColor.name();

	setStyleSheet(
		stylesheetTemplate
			.arg(_textSize)
			.arg(textColorHex)
			.arg(bgColorHex)
	);
}
