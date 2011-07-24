/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef SPRINGLOBBY_HEADERGUARD_ENGINECONFIG_H
#define SPRINGLOBBY_HEADERGUARD_ENGINECONFIG_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <map>
#include <string>

class EngineConfig;

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
	int lastSessionIndex() const;

private:
	void reload();
	typedef QList<std::pair<EngineConfig,QString> >
		ContainerType;
	ContainerType presets_;
	int last_sessions_index_;
};

//! essential config read stuff from engine
class EngineConfig
{
public:
	EngineConfig();
	EngineConfig( const QString& filename, const QString& id );
	bool load( const QString& filename, const QString& id );
	void commit() const;
	QString filename();

private:
	// helper functions
	void Read(FILE* file);
	void AppendLine(char* line);

	QString filename_;
	QString id_;
	typedef std::map<std::string, std::string>
		DataContainerType;
	DataContainerType data_;
};

class ScreenResolutionModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ScreenResolutionModel(QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
	QString name(int index) const;
	void use(int index) const;
	int lastSessionIndex() const;

private:
	struct ScreenResolution {
		ScreenResolution( const int w, const int h, const bool dual )
			:width(w),height(h),dualscreen(dual) {}
		const int width;
		const int height;
		const bool dualscreen;
		QString toString() const;
	};
	void reload();
	typedef QList<ScreenResolution>
		ContainerType;
	ContainerType resolutions_;
	int last_sessions_index_;
};

#endif // SPRINGLOBBY_HEADERGUARD_ENGINECONFIG_H
