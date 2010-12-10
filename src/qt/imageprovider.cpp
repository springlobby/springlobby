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
#include <qt/converters.h>
#include <utils/conversion.h>
#include <wx/image.h>
#include <QDebug>

QImage ImageProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	int width = requestedSize.width() > 0 ? requestedSize.width() : 1024;
	int height = requestedSize.height() > 0 ? requestedSize.height() : 1024;

	wxImage h = SLcustomizations().GetBackgroundImage();
	wxSize k_size = SLcustomizations().GetBackgroundSize();

	if (size)
		*size = wxQtConvertSize( k_size );
	qDebug() << height << width ;
	QImage q = wxQtConvertImage( h );

//	q = q.scaled(requestedSize);
	assert( !q.isNull() );

	return q.scaled( *size );
}


QImage MinimapImageProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	int width = requestedSize.width() > 0 ? requestedSize.width() : 1024;
	int height = requestedSize.height() > 0 ? requestedSize.height() : 1024;

	wxImage h = usync().GetMinimap( TowxString( id.toStdString() ), width, height );
	if (size)
		*size = QSize(width,height);
	QImage q = wxQtConvertImage( h );
	assert( !q.isNull() );
	return q.scaled( width, height );
}

QImage SideImageProvider::requestImage ( const QString & id, QSize * size, const QSize & requestedSize )
{
	int width = requestedSize.width() > 0 ? requestedSize.width() : 16;
	int height = requestedSize.height() > 0 ? requestedSize.height() : 16;
	const wxString& modname = SLcustomizations().GetModname();

	wxImage h = usync().GetSidePicture( modname, TowxString( id.toStdString() ) );
	if (size)
		*size = QSize(width,height);
	QImage q = wxQtConvertImage( h );
	assert( !q.isNull() );
	return q.scaled( width, height );
}
