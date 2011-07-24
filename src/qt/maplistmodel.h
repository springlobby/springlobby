/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

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


#ifndef MAPLISTMODEL_H
#define MAPLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>

class wxArrayString;

class MaplistModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit MaplistModel(const wxArrayString& map_names, QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	void reload( const wxArrayString& map_names );

public slots:
	QString name(int index) const;

private:
	typedef QStringList
		ContainerType;
	ContainerType m_map_names;
};

#endif // MAPLISTMODEL_H
