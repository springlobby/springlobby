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
	int width = 100;
	int height = 50;


//	QImage pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
//				   requestedSize.height() > 0 ? requestedSize.height() : height);
//	pixmap.fill(QColor(id).rgba());

//	wxImage h = SLcustomizations().GetBackgroundImage();
	wxImage h = usync().GetMinimap( TowxString( id.toStdString() ) );
//	wxSize k_size = SLcustomizations().GetBackgroundSize();
//	assert( k_size != wxDefaultSize );
	if (size)
		*size = requestedSize;
	qDebug() << h.GetWidth() << h.GetHeight() ;
	QImage q = wxQtConvertImage( h );

//	q = q.scaled(requestedSize);
	assert( !q.isNull() );

	return q;
}
