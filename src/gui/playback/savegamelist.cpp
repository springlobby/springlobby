/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "savegamelist.h"

#include <iterator>
#include <algorithm>
#include <fstream>
#include <wx/file.h>
#include <wx/filefn.h>

#include "gui/customdialogs.h"
#include "playbacktab.h"
#include "playbackstructs.h"
#include "gui/uiutils.h"
#include <lslutils/globalsmanager.h>


SavegameList::SavegameList()
{
}


void SavegameList::LoadPlaybacks(const std::vector<std::string> &filenames )
{
    m_fails = 0;

    m_replays.clear();
    const size_t size = filenames.size();
    for (size_t i = 0; i < size; ++i)
    {
        const wxString fn = TowxString(filenames[i]);
		Savegame& rep_ref = AddPlayback( i ); // don't touch this reference, since elements inside this data structure are filled using pointers, adding & not fecthing the new addresses would screw up references when rep gets destroyed

        if (!GetSavegameInfos( fn, rep_ref))
        {
			RemovePlayback( rep_ref.id );
            m_fails++;
        }
    }
}

bool SavegameList::GetSavegameInfos ( const wxString& SavegamePath, Savegame& ret ) const
{
    //wxLogMessage(_T("GetSavegameInfos %s"), SavegamePath.c_str());
    //wxLOG_Info  ( STD_STRING( SavegamePath ) );
    //TODO extract moar info
    ret.Filename = SavegamePath;
    ret.battle.SetPlayBackFilePath(STD_STRING(SavegamePath));
    if ( SavegamePath.IsEmpty() )
        return false;
    ret.battle.SetScript( STD_STRING(GetScriptFromSavegame( SavegamePath )));
    //wxLogMessage(_T("Script: %s"), script.c_str());

    if ( ret.battle.GetScript().empty() )
        return false;

    ret.battle.GetBattleFromScript( false );
    ret.ModName = TowxString(ret.battle.GetHostModName());
    ret.battle.SetBattleType( BT_Savegame );
	ret.size = wxFileName::GetSize( SavegamePath ).ToULong();

    return true;
}

wxString SavegameList::GetScriptFromSavegame ( const wxString& SavegamePath  ) const
{
	// blatantly copied from spring source
	std::ifstream file(SavegamePath.mb_str(), std::ios::in|std::ios::binary);
	std::string script;
	char c;
	if ( file.is_open() )
	{
		do
		{
			file.read(&c,sizeof(char));
			if (c) script += c;
		} while ( ( c != 0 ) && !file.fail() );
	}
	return TowxString( script );
}
