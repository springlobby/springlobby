/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#include "imageprovider.h"

QImage ImageProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	int width = 100;
	int height = 50;

	if (size)
		*size = QSize(width, height);
//	QImage pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
//				   requestedSize.height() > 0 ? requestedSize.height() : height);
//	pixmap.fill(QColor(id).rgba());

	return QImage();
}
