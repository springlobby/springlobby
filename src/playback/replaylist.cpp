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
	std::string datadir;
	m_replays.clear();
	const size_t size = filenames.size();
	for ( size_t i = 0; i < size; ++i) {
		const wxString wfilename = TowxString(filenames[i]);
		PlaybackType& playback = AddPlayback(i);
		if (!GetReplayInfos(wfilename, playback)) {
			wxLogError(_T("Couldn't open replay %s"), wfilename.c_str() );
			RemovePlayback(i); //FIXME: stupid logic: always add but remove on fail, why not add on success only?
		}
	}
}


int ReplayList::replayVersion(wxFile& replay ) const
{
	if(replay.Seek(16)==wxInvalidOffset) {
		return 0;
	}
	int version = 0;
	replay.Read( &version, 4);
	return version;
}

bool ReplayList::GetReplayInfos(const wxString& ReplayPath, Replay& ret ) const
{
	const wxString FileName = ReplayPath.AfterLast( wxFileName::GetPathSeparator() ); // strips file path
	ret.Filename = ReplayPath;
	ret.battle.SetPlayBackFilePath( ReplayPath );
	ret.SpringVersion = FileName.AfterLast(_T('_')).BeforeLast(_T('.'));
	ret.MapName = FileName.BeforeLast(_T('_'));

	if (!wxFileExists(ReplayPath)) {
		return false;
	}
	wxFile replay(ReplayPath, wxFile::read );
	if (!replay.IsOpened()) {
		return false;
	}

	const int replay_version = replayVersion( replay );
	ret.battle.SetScript( GetScriptFromReplay( replay, replay_version ) );

	if ( ret.battle.GetScript().IsEmpty() ) {
		return false;
	}

	GetHeaderInfo(replay, ret, replay_version );
	ret.battle.GetBattleFromScript( false );
	ret.ModName = ret.battle.GetHostModName();
	ret.battle.SetBattleType( BT_Replay );

	//getting this from filename seems more reliable than from demoheader
	wxDateTime rdate;

	if (rdate.ParseFormat(FileName, _T("%Y%m%d_%H%M%S")) == 0) {
		wxLogError(_T("Parsing %s failed"), FileName.c_str());
		return false;
	}
	ret.date=rdate.GetTicks(); // now it is sorted properly
	ret.date_string=rdate.FormatISODate()+_T(" ")+rdate.FormatISOTime();

	return true;
}

wxString ReplayList::GetScriptFromReplay(wxFile& replay, const int version) const
{

	wxString script;
	if ( !replay.IsOpened() ) return script;
	if(replay.Seek(20)==wxInvalidOffset) {
		return script;
	}
	int headerSize=0 ;
	replay.Read( &headerSize, 4);
	const int seek = 64 + (version < 5 ? 0 : 240);
	if(replay.Seek(seek)==wxInvalidOffset) {
		return script;
	}
	wxFileOffset scriptSize=0;
	replay.Read( &scriptSize, 4);
	scriptSize = LSL::Util::Clamp( wxFileOffset(scriptSize), wxFileOffset(0), replay.Length() );
	if(replay.Seek(headerSize) == wxInvalidOffset)return script;
	std::string script_a(scriptSize,0);
	replay.Read( &script_a[0], scriptSize );
	script = TowxString( script_a ) ;//(script_a,scriptSize);
	return script;
}

void ReplayList::GetHeaderInfo(wxFile& replay, Replay& rep, const int version) const
{
	const int seek = 72 + (version < 5 ? 0 : 240);
	if(replay.Seek(seek)==wxInvalidOffset) {
		return;
	}
	int gametime = 0 ;
	replay.Read( &gametime, 4);
	rep.duration = gametime;
	rep.size = replay.Length();
	//! \todo don't use long long? (pedantic)
	wxLongLong_t unixtime = 0;
	if(replay.Seek(56)==wxInvalidOffset) {
		return;
	}
	replay.Read( &unixtime, 8 );
	wxDateTime dt;
	dt.Set( (time_t) unixtime );
	// todo: add 2 strings one for date other for time?
	wxString date_string = dt.FormatISODate()+_T(" ")+dt.FormatISOTime();
	//  rep.date = (time_t) unixtime ;
	rep.date_string = date_string;
}
