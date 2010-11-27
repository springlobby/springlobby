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


#include "maplistmodel.h"

#include <wx/arrstr.h>

MaplistModel::MaplistModel(const wxArrayString& map_names, QObject *parent )
	:QAbstractListModel( parent )
{
	wxString mapname;
	foreach (mapname, map_names ) {
		m_map_names.append( QString(mapname.mb_str()) );
	}
}

int MaplistModel::rowCount(const QModelIndex &parent ) const
{
	return m_map_names.size();
}

QVariant MaplistModel::data(const QModelIndex &index, int role ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= m_map_names.size() )
		   return QVariant();
	return QVariant::fromValue(m_map_names[row]);
}

QString MaplistModel::name(int index) const
{
	if ( index < m_map_names.size() )
		return m_map_names[index];
	return QString();
}
