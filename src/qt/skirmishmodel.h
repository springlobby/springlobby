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


#ifndef SKIRMISHMODEL_H
#define SKIRMISHMODEL_H

#include <QAbstractListModel>
#include "../mmoptionswrapper.h"
#include <qt/noguisingleplayerbattle.h>

class SkirmishModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SkirmishModel(QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:

public slots:

private:
	OptionsWrapper m_mod_customs;

	NoGuiSinglePlayerBattle m_battle;

	typedef std::vector< std::pair<wxString, OptionsWrapper> >
		SkirmishMap;

	SkirmishMap m_skirmishes;

};

#endif // SKIRMISHMODEL_H
