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

#include <tasserver.h>
#include <iserverevents.h>
#include <settings.h>

BattlelistModel::BattlelistModel(const wxString& modname, QObject *parent)
    : QAbstractListModel(parent),
      m_modname( modname ),
      m_server( 0 )
{
    m_server = new TASServer( IServerEvents::simple );
    serverSelector().SetCurrentServer( m_server );
    const wxString servername = sett().GetDefaultServer();
    const wxString username = sett().GetServerAccountNick( servername );
    const wxString password = sett().GetServerAccountPass( servername );

    serverSelector().GetServer().SetUsername( username );
    serverSelector().GetServer().SetPassword( password );
    if ( sett().GetServerAccountSavePass( servername ) )
    {
        if ( serverSelector().GetServer().IsPasswordHash(password) ) sett().SetServerAccountPass( servername, password );
        else sett().SetServerAccountPass( servername, serverSelector().GetServer().GetPasswordHash( password ) );
    }
    else
    {
        sett().SetServerAccountPass( servername, _T("") );
    }
    const wxString host = sett().GetServerHost( servername );
    const int port = sett().GetServerPort( servername );
    serverSelector().GetServer().Connect( servername, host, port );
}

BattlelistModel::~BattlelistModel()
{
    serverSelector().GetServer().Disconnect();
}

int BattlelistModel::rowCount(const QModelIndex&/*parent*/ ) const
{
    return m_battles.size();
}

QVariant BattlelistModel::data(const QModelIndex &index, int role ) const
{
    int row =  index.row();
    if ( !index.isValid() || row >= m_battles.size() )
        return QVariant();
    return FromwxString<QVariant>( m_battles[row]->GetDescription() );
}

void BattlelistModel::reload()
{
    m_battles.clear();
    BattleList_Iter* iter = (serverSelector().GetServer().battles_iter);
    if ( !iter )
        return;
    iter->IteratorBegin();
    while( !(iter->EOL()) ) {
        m_battles.append( iter->GetBattle() );
    }
}
