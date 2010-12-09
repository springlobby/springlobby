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


#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class ImageProvider : public QDeclarativeImageProvider
{
public:
	ImageProvider()
		: QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
	  { }

	virtual QImage requestImage ( const QString & id, QSize * size, const QSize & requestedSize );
	QDeclarativeImageProvider::ImageType imageType () const { return QDeclarativeImageProvider::Image; }
};

class MinimapImageProvider : public QDeclarativeImageProvider
{
public:
	MinimapImageProvider()
		: QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
	  { }

	  virtual QImage requestImage ( const QString & id, QSize * size, const QSize & requestedSize );
		QDeclarativeImageProvider::ImageType imageType () const { return QDeclarativeImageProvider::Image; }
};


class SideImageProvider : public QDeclarativeImageProvider
{
public:
	SideImageProvider()
		: QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
	  { }

	  virtual QImage requestImage ( const QString & id, QSize * size, const QSize & requestedSize );
		QDeclarativeImageProvider::ImageType imageType () const { return QDeclarativeImageProvider::Image; }
};

#endif // IMAGEPROVIDER_H
