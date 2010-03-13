#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H

#include "ibattle.h"
#include "user.h"

class Ui;
class BattleBot;
class MainSinglePlayerTab;
class wxColour;
class wxString;


class SinglePlayerBattle: public IBattle
{
  public:

    SinglePlayerBattle( MainSinglePlayerTab& msptab );
    virtual ~SinglePlayerBattle();

// (koshi) these are never called
//    unsigned int AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll );
//    void UpdateBot( unsigned int index, int ally, int posx, int posy, int side );

    bool IsFounderMe() { return true; }

    User& GetMe() { return m_me; }
    const User& GetMe() const { return m_me; }

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& /*unused*/ ){ }

    void Update( const wxString& Tag );

    void StartSpring();

  protected:
    void RemoveUnfittingBots();

    MainSinglePlayerTab& m_sptab;

    User m_me;

};

/** A derivative used in the Simple interface

    Has no GUI tie-ins like SingelPlayerBattle and provides some functions to translate from the mod customization settings to our IBattle logic.

**/
class NoGuiSinglePlayerBattle : public IBattle {
    public:
        NoGuiSinglePlayerBattle ();
        virtual ~NoGuiSinglePlayerBattle () {}
        void StartSpring();
        bool IsFounderMe() { return true; }
        User& GetMe() { return m_me; }
        const User& GetMe() const { return m_me; }

        bool AddBot( const wxString& name, int team_id, const wxString& side );

        //! returns -1 on fail, otherwise usnyc index that matches the given name
        int GetSideIndex( const wxString& name );

    protected:
        //! returns -1 on fail, otherwise usnyc index that matches the given name
        int GetAiIndex( const wxString& name );

        User m_me;
};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

