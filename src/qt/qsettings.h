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


#ifndef SPRINGLOBBY_HEADERGUARD_QSETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_QSETTINGS_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <map>
#include <string>

class PresetModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit PresetModel(QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
	QString name(int index) const;
	void use(int index) const;

private:
	void reload();
	typedef QStringList
		ContainerType;
	ContainerType preset_names_;
};

//! essential config read stuff from engine
class QSettings
{
public:
	QSettings();
	bool load( const QString& filename );
	void commit();

private:

	// helper functions
	void Read(FILE* file);
	void AppendLine(char* line);

	QString filename_;
	std::map<std::string, std::string> data_;
};


#endif // SPRINGLOBBY_HEADERGUARD_QSETTINGS_H
