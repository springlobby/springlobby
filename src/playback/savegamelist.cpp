#include "savegamelist.h"

/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <iterator>
#include <algorithm>
#include <wx/file.h>
#include <wx/filefn.h>

#include "replaylist.h"
#include "../utils.h"
#include "../settings++/custom_dialogs.h"
#include "playbacktab.h"
#include "playbackstructs.h"
#include "../uiutils.h"
#include "../globalsmanager.h"
#include "scriptparser.h"


SavegameList::SavegameList()
{
}


void SavegameList::LoadPlaybacks( const wxArrayString& filenames )
{
    m_fails = 0;

    m_replays.clear();
    size_t size = filenames.GetCount();
    for ( size_t i = 0; i < size; ++i)
    {
        Savegame rep;
        rep.id = i;
        wxString fn = filenames[i];
        Savegame& rep_ref = AddPlayback( rep ); // don't touch this reference, since elements inside this data structure are filled using pointers, adding & not fecthing the new addresses would screw up references when rep gets destroyed

        if ( !GetSavegameInfos( filenames[i] , rep_ref ) )
        {
            RemovePlayback( rep.id );
            m_fails++;
        }
    }
}

bool SavegameList::GetSavegameInfos ( const wxString& SavegamePath, Savegame& ret )
{
    //wxLogMessage(_T("GetSavegameInfos %s"), SavegamePath.c_str());
    //wxLOG_Info  ( STD_STRING( SavegamePath ) );
    //TODO extract moar info
    ret.Filename = SavegamePath;
		ret.battle.SetPlayBackFilePath( SavegamePath );

		wxString script = GetScriptFromSavegame( SavegamePath );
    ret.battle.SetScript( script );
    //wxLogMessage(_T("Script: %s"), script.c_str());

    if ( script.IsEmpty() ) return false;

    GetBattleFromScript( ret.battle, false  );
    ret.ModName = ret.battle.GetHostModName();
    ret.battle.SetBattleType( BT_Savegame );

    return true;
}

wxString SavegameList::GetScriptFromSavegame ( const wxString& SavegamePath  )
{
	wxString ret;
	try
	{
			wxFile save( SavegamePath, wxFile::read );

			std::string script_a( save.Length(), '\1' );
			save.Read( &script_a[0], save.Length() );
			size_t pos = script_a.find( "\0" );
			if ( pos != -1 ) ret = WX_STRING( script_a.substr( 0, pos ) );
	}
	catch (...)
	{
	}
	return ret;
}
