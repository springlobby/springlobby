/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <iterator>
#include <algorithm>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/datetime.h>

#include "replaylist.h"
#include <lslutils/misc.h>
#include "utils/conversion.h"
#include "utils/customdialogs.h"
#include "playbacktab.h"
#include "playbackstructs.h"
#include "uiutils.h"
#include <lslutils/globalsmanager.h>

ReplayList::ReplayList()
{
}

void ReplayList::LoadPlaybacks(const std::vector<std::string> &filenames )
{
	m_replays.clear();
	for ( size_t i = 0; i < filenames.size(); ++i) {
		const wxString wfilename = TowxString(filenames[i]);
		Replay repl;
		repl.id = i;
		if (GetReplayInfos(wfilename, repl)) {
			AddPlayback(repl, i);
			wxLogDebug(_T("-------------- %d %s %s"),i, TowxString(repl.battle.GetHostModName()).c_str(), TowxString(repl.battle.GetHostMapName()).c_str());
		} else {
			wxLogDebug(_T("Error adding %s"), wfilename.c_str());
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
	ret.battle.SetPlayBackFilePath(STD_STRING(ReplayPath));
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
	ret.battle.SetScript(STD_STRING(GetScriptFromReplay( replay, replay_version )));

	if ( ret.battle.GetScript().empty() ) {
		return false;
	}

	GetHeaderInfo(replay, ret, replay_version );
	ret.battle.GetBattleFromScript( false );
	ret.ModName = TowxString(ret.battle.GetHostModName());
	ret.battle.SetBattleType( BT_Replay );
	ret.battle.SetEngineName("spring");
	ret.battle.SetEngineVersion(STD_STRING(ret.SpringVersion));
	ret.battle.SetPlayBackFilePath(STD_STRING(ReplayPath));

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
