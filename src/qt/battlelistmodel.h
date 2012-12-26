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


#ifndef BATTLELISTMODEL_H
#define BATTLELISTMODEL_H

#ifdef SL_QT_MODE

#include <QAbstractListModel>
#include <QList>

class wxString;
class Battle;

class BattlelistModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit BattlelistModel( const wxString& modname, QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	void reload();
	~BattlelistModel();
        enum Roles {
                         Description = Qt::UserRole + 1,
						 Mapname,Founder,PlayerMax,PlayerCurrent,BattleId
                 };
public slots:
        int battle_count();
private:
	const wxString& m_modname;
	QList<Battle*> m_battles;
};

#endif // #ifdef SL_QT_MODE
#endif // BATTLELISTMODEL_H
