#ifndef SAVEGAMELIST_H
#define SAVEGAMELIST_H

#include "playbacklist.h"

struct Savegame;

template <class PB, class T>
class GlobalObjectHolder;

class SavegameList : public PlaybackList<Savegame>
{
  public:

    typedef Savegame
        PlaybackType;

   virtual  void LoadPlaybacks( const wxArrayString& filenames );

  protected:
    SavegameList();

    template <class PB, class I >
    friend class GlobalObjectHolder;

	bool GetSavegameInfos ( const wxString& SavegamePath, Savegame& ret ) const;
	wxString GetScriptFromSavegame ( const wxString& SavegamePath ) const;
    BattleOptions GetBattleOptsFromScript( const wxString& script_ );

    //! load mod/map options
    void LoadMMOpts( const wxString& sectionname, OfflineBattle& battle, const PDataList& node );
    //! load engine options
    void LoadMMOpts( OfflineBattle& battle, const PDataList& node );

    //! saves relevant infos from header into replay struct
    void GetHeaderInfo( Savegame& rep, const wxString& SavegamePath );

};

#endif // SAVEGAMELIST_H

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
