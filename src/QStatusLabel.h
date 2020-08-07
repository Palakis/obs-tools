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

#include <QtCore/QString>
#include <QtWidgets/QLabel>
#include <QtGui/QColor>

class QStatusLabel : public QLabel
{
Q_OBJECT
public:
	explicit QStatusLabel(const QString& offAirText, const QString& onAirText, QWidget* parent = nullptr);
	void setStatus(const QString& text, const QColor& textColor, const QColor& bgColor);
	void turnOn();
	void turnOff();

protected:
	void resizeEvent(QResizeEvent* ev) override;

private:
	const QString computeTextSize(const QSize& size);
	void refreshStyleSheet();

	const QString _offText;
	const QString _onText;
	QString _textSize;
	QColor _textColor;
	QColor _bgColor;
};
