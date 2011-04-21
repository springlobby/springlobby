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


#include "sidemodel.h"
#include <springunitsync.h>

SideModel::SideModel(const wxString& modname, QObject *parent) :
    QAbstractListModel(parent)
{
	wxString name;
	foreach( name, usync().GetSides( modname ) )
		m_sidenames.append( ToQString( name ) );
}

int SideModel::rowCount(const QModelIndex &parent ) const
{
	return m_sidenames.size();
}

QVariant SideModel::data(const QModelIndex &index, int role ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= m_sidenames.size() )
		   return QVariant();
	return QVariant::fromValue( m_sidenames[row] );
}
