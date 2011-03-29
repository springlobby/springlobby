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


#include "qsettings.h"

#include <QDir>

#include <customizations.h>

QSettings::QSettings()
{
}



PresetModel::PresetModel(QObject *parent )
	:QAbstractListModel( parent )
{
	reload( );
}

void PresetModel::reload()
{
	preset_names_.clear();
	QDir preset_dir( SLcustomizations().DataBasePath() + "/presets/" );
//	wxString mapname;
//	foreach (mapname, map_names ) {
//		m_map_names.append( QString(mapname.mb_str()) );
//	}
	QStringList nameFilter;
		nameFilter << "*.conf";
	QFileInfo current_file;
	foreach( current_file, preset_dir.entryInfoList( nameFilter, QDir::Files ) )
	{
		 preset_names_.append( current_file.fileName().replace(".conf","") );
	}
}

int PresetModel::rowCount(const QModelIndex &parent ) const
{
	return preset_names_.size();
}

QVariant PresetModel::data(const QModelIndex &index, int /*role*/ ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= preset_names_.size() )
		   return QVariant();
	return QVariant::fromValue(preset_names_[row]);
}

QString PresetModel::name(int index) const
{
	if ( index < preset_names_.size() )
		return preset_names_[index];
	return QString();
}
