/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <iterator>
#include <algorithm>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/datetime.h>

#include "replaylist.h"
#include <lslutils/misc.h>
#include "../utils/conversion.h"
#include "../utils/customdialogs.h"
#include "playbacktab.h"
#include "playbackstructs.h"
#include "../uiutils.h"
#include <lslutils/globalsmanager.h>


ReplayList::ReplayList()
{
}

void ReplayList::LoadPlaybacks(const std::vector<std::string> &filenames )
{
    m_fails = 0;

    m_replays.clear();
    const size_t size = filenames.size();
    for ( size_t i = 0; i < size; ++i)
    {
		Replay& rep_ref = AddPlayback( i ); // don't touch this reference, since elements inside this data structure are filled using pointers, adding & not fecthing the new addresses would screw up references when rep gets destroyed
        if ( !GetReplayInfos(TowxString(filenames[i]), rep_ref ) )
        {
			RemovePlayback( rep_ref.id );
            m_fails++;
        }
    }
}


#define SEEK(x) if(replay.Seek(x)==wxInvalidOffset)return 0;
int replayVersion( const wxString& ReplayPath )
{
    wxFile replay( ReplayPath, wxFile::read );
    if ( !replay.IsOpened() ) return 0;
    SEEK( 16 );
    int version = 0;
    replay.Read( &version, 4);
    return version;
}
#undef SEEK

bool ReplayList::GetReplayInfos (const wxString& ReplayPath, Replay& ret ) const
{
	const wxString FileName = ReplayPath.AfterLast( wxFileName::GetPathSeparator() ); // strips file path
	ret.Filename = ReplayPath;
	ret.battle.SetPlayBackFilePath( ReplayPath );
	ret.SpringVersion = FileName.AfterLast(_T('_')).BeforeLast(_T('.'));
	ret.MapName = FileName.BeforeLast(_T('_'));

	const int replay_version = replayVersion( ReplayPath );
	ret.battle.SetScript( GetScriptFromReplay( ReplayPath, replay_version ) );

	if ( ret.battle.GetScript().IsEmpty() ) return false;

	GetHeaderInfo( ret, ReplayPath, replay_version );
	ret.battle.GetBattleFromScript( false );
	ret.ModName = ret.battle.GetHostModName();
	ret.battle.SetBattleType( BT_Replay );

	//getting this from filename seems more reliable than from demoheader
	wxDateTime rdate;

	if (rdate.ParseFormat(FileName, _T("%Y%m%d_%H%M%S")) == NULL) {
		wxLogError(_T("Parsing %s failed"), FileName.c_str());
		return false;
	}
	ret.date=rdate.GetTicks(); // now it is sorted properly
	ret.date_string=rdate.FormatISODate()+_T(" ")+rdate.FormatISOTime();

	return true;
}

#define SEEK(x) if(replay.Seek(x)==wxInvalidOffset)return script;
wxString ReplayList::GetScriptFromReplay (const wxString& ReplayPath  , const int version) const
{

	wxString script;
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        if ( !replay.IsOpened() ) return script;
        SEEK( 20 );
        int headerSize=0 ;
        replay.Read( &headerSize, 4);
        const int seek = 64 + (version < 5 ? 0 : 240);
		SEEK( seek );
		wxFileOffset scriptSize=0;
        replay.Read( &scriptSize, 4);
		scriptSize = LSL::Util::Clamp( wxFileOffset(scriptSize), wxFileOffset(0), replay.Length() );
		SEEK( headerSize );
        std::string script_a(scriptSize,0);
        replay.Read( &script_a[0], scriptSize );
        script = TowxString( script_a ) ;//(script_a,scriptSize);

    }
    catch (...)
    {
    }
	return script;

}
#undef SEEK
#define SEEK(x) if(replay.Seek(x)==wxInvalidOffset)return;
void ReplayList::GetHeaderInfo(Replay& rep, const wxString& ReplayPath , const int version) const
{
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        const int seek = 72 + (version < 5 ? 0 : 240);
        SEEK( seek );
        int gametime = 0 ;
        replay.Read( &gametime, 4);
        rep.duration = gametime;
        rep.size = replay.Length();
		//! \todo don't use long long? (pedantic)
		wxLongLong_t unixtime = 0;
		SEEK( 56 );
        replay.Read( &unixtime, 8 );
        wxDateTime dt;
        dt.Set( (time_t) unixtime );
        // todo: add 2 strings one for date other for time?
        wxString date_string = dt.FormatISODate()+_T(" ")+dt.FormatISOTime();
      //  rep.date = (time_t) unixtime ;
        rep.date_string = date_string;
    }
    catch (...){ }
}

#undef SEEK
