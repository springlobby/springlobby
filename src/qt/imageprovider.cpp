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
#include <customizations.h>
#include <springunitsync.h>
#include <qt/converters.h>
#include <utils/conversion.h>
#include <wx/image.h>
#include <QDebug>

QImage GraphicsProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	QImage img( SLcustomizations().GraphicsDir() + "/" + id );//.scaled( requestedSize );
	int width = requestedSize.width() > 0 ? requestedSize.width() : img.width();
	int height = requestedSize.height() > 0 ? requestedSize.height() : img.height();
	if (size)
		*size = QSize(width,height);
	return img.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}


QImage MinimapImageProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	int width = requestedSize.width() > 0 ? requestedSize.width() : 1024;
	int height = requestedSize.height() > 0 ? requestedSize.height() : 1024;

	const wxImage h = usync().GetMinimap( TowxString( id ), width, height );
	if (size)
		*size = QSize(width,height);
	const QImage q = wxQtConvertImage( h );
	assert( !q.isNull() );
	return q.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}

QImage SideImageProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	int width = requestedSize.width() > 0 ? requestedSize.width() : 16;
	int height = requestedSize.height() > 0 ? requestedSize.height() : 16;
	if (size)
		*size = QSize(width,height);

	QImage img;
	if ( !img.load( SLcustomizations().GraphicsDir() + "/sidepics/" + id + ".png" ) )
		img.load( SLcustomizations().GraphicsDir() + "/sidepics/" + id.toLower() + ".png" );

	return img.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}
