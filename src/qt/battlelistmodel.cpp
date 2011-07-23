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


#include "battlelistmodel.h"


#include <iserverevents.h>
#include <settings.h>
#include <server.h>

BattlelistModel::BattlelistModel(const wxString& modname, QObject *parent)
    : QAbstractListModel(parent),
      m_modname( modname )
{
}

BattlelistModel::~BattlelistModel()
{}

int BattlelistModel::rowCount(const QModelIndex&/*parent*/ ) const
{
    return m_battles.size();
}

QVariant BattlelistModel::data(const QModelIndex &index, int role ) const
{
    int row =  index.row();
    if ( !index.isValid() || row >= m_battles.size() || !(m_battles[row]) )
        return QVariant();
    const Battle& battle = *m_battles[row];
    return FromwxString<QVariant>( battle.GetHostModName()
                + _T(" - ") + battle.GetHostMapName()
                + _T(" - ") + battle.GetFounder().GetNick()  );
}

void BattlelistModel::reload()
{
    m_battles.clear();
    BattleList_Iter* iter = (serverSelector().GetServer().battles_iter);
    if ( !iter )
        return;
    iter->IteratorBegin();
    beginInsertRows(QModelIndex(), 0, iter->GetNumBattles() );
    while( !(iter->EOL()) ) {
        m_battles.append( iter->GetBattle() );
    }
    endInsertRows();
}

int BattlelistModel::battle_count()
{
    BattleList_Iter* iter = (serverSelector().GetServer().battles_iter);
    return iter ? iter->GetNumBattles() : -1;
}
