/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <iterator>
#include <algorithm>
#include <wx/file.h>
#include <wx/filefn.h>

#include "replaylist.h"
#include "../utils/math.h"
#include "../utils/conversion.h"
#include "../utils/customdialogs.h"
#include "playbacktab.h"
#include "playbackstructs.h"
#include "../uiutils.h"
#include "../globalsmanager.h"


ReplayList::ReplayList()
{
}

void ReplayList::LoadPlaybacks( const wxArrayString& filenames )
{
    m_fails = 0;

    m_replays.clear();
    size_t size = filenames.GetCount();
    for ( size_t i = 0; i < size; ++i)
    {
        Replay rep;
        rep.id = i;
				Replay& rep_ref = AddPlayback( rep ); // don't touch this reference, since elements inside this data structure are filled using pointers, adding & not fecthing the new addresses would screw up references when rep gets destroyed
        if ( !GetReplayInfos( filenames[i] , rep_ref ) )
        {
            RemovePlayback( rep.id );
            m_fails++;
        }
    }
}

bool ReplayList::GetReplayInfos ( const wxString& ReplayPath, Replay& ret )
{
    //wxLogMessage(_T("GetReplayInfos %s"), ReplayPath.c_str());
    //wxLOG_Info  ( STD_STRING( ReplayPath ) );
    //TODO extract moar info
    ret.Filename = ReplayPath;
    ret.battle.SetPlayBackFilePath( ReplayPath );

	wxString FileName = ReplayPath.AfterLast( wxFileName::GetPathSeparator() ); // strips file path
    FileName = FileName.BeforeLast( _T('.') ); //strips the file extension;

    ret.date_string = FileName.BeforeFirst(_T('_'));
    FileName = FileName.AfterFirst(_T('_'));

    FileName = FileName.AfterFirst(_T('_')); // strips hours minutes seconds informatiom

    ret.SpringVersion = FileName.AfterLast(_T('_'));

    ret.MapName = FileName.BeforeLast(_T('_'));

    ret.battle.SetScript( GetScriptFromReplay( ReplayPath ) );
    //wxLogMessage(_T("Script: %s"), script.c_str());

    if ( ret.battle.GetScript().IsEmpty() ) return false;

    GetHeaderInfo( ret, ReplayPath );
    ret.battle.GetBattleFromScript( false );
    ret.ModName = ret.battle.GetHostModName();
    ret.battle.SetBattleType( BT_Replay );

    return true;
}

wxString ReplayList::GetScriptFromReplay ( const wxString& ReplayPath  )
{
	wxString script;
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        if ( !replay.IsOpened() ) return script;
        replay.Seek( 20 );
        int headerSize=0 ;
        replay.Read( &headerSize, 4);
        replay.Seek( 64 );
		wxFileOffset scriptSize=0;
        replay.Read( &scriptSize, 4);
		scriptSize = clamp( wxFileOffset(scriptSize), wxFileOffset(0), replay.Length() );
        replay.Seek( headerSize );
        std::string script_a(scriptSize,0);
        replay.Read( &script_a[0], scriptSize );
        script = TowxString( script_a ) ;//(script_a,scriptSize);

    }
    catch (...)
    {
    }
	return script;
}


void ReplayList::GetHeaderInfo( Replay& rep, const wxString& ReplayPath )
{
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        replay.Seek( 72 );
        int gametime = 0 ;
        replay.Read( &gametime, 4);
        rep.duration = gametime;
        rep.size = replay.Length();
		wxLongLong_t unixtime = 0;
        replay.Seek( 56 );
        replay.Read( &unixtime, 8 );
        wxDateTime dt;
        dt.Set( (time_t) unixtime );
        // todo: add 2 strings one for date other for time?
        wxString date_string = dt.FormatISODate()+_T(" ")+dt.FormatISOTime();
        rep.date = (time_t) unixtime ;
        rep.date_string = date_string;
    }
    catch (...){ }
}

