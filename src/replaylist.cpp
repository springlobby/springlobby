/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <wx/datetime.h>

#include "replaylist.h"
#include "storedgame.h"
#include "utils/conversion.h"
#include <lslutils/globalsmanager.h>

IPlaybackList& replaylist()
{
	static LSL::Util::LineInfo<ReplayList> m(AT);
	static LSL::Util::GlobalObjectHolder<ReplayList, LSL::Util::LineInfo<ReplayList> > m_replay_list(m);
	return m_replay_list;
}

ReplayList::ReplayList()
{
}

int ReplayList::FindPlayback(const std::string& filename) const //returns id when the filename already exists in m_replays
{
	for (const auto& playback: m_replays){
        if (playback.second.Filename == filename)
			return playback.first;
	}
	return -1;
}

bool ReplayList::FindFilename(const std::vector<std::string>& filenames, const std::string& filename) const //search
{
	for (size_t i = 0; i < filenames.size(); ++i) {
		if (filenames[i] == filename)
			return true;
	}
	return false;
}

void ReplayList::LoadPlaybacks(const std::vector<std::string>& filenames)
{
	for (size_t i = 0; i < filenames.size(); ++i) { //add replays which doesn't exist yet
		const std::string filename = filenames[i];
		const int pos = FindPlayback(filename);
		if (pos != -1) {
			StoredGame& playback = AddPlayback(i);
			GetReplayInfos(filename, playback);
		}
	}

	std::list<unsigned int> todel;
	for (const auto& playback: m_replays){ //remove not re-added playbacks (deleted?!)
		if (!FindFilename(filenames, playback.second.Filename)) {
			todel.push_back(playback.first);
		}
	}

	for (unsigned int id: todel) {
			DeletePlayback(id);
	}

	assert(m_replays.size() == filenames.size());
}


int ReplayList::replayVersion(wxFile& replay) const
{
	if (replay.Seek(16) == wxInvalidOffset) {
		return 0;
	}
	int version = 0;
	replay.Read(&version, 4);
	return version;
}

static void MarkBroken(StoredGame& ret)
{
	ret.battle.SetHostMap("broken", "");
	ret.battle.SetHostGame("broken", "");
}

bool ReplayList::GetReplayInfos(const std::string& ReplayPath, StoredGame& ret) const
{
	const std::string FileName = LSL::Util::AfterLast(ReplayPath, SEP); // strips file path
	ret.type = StoredGame::REPLAY;
	ret.Filename = ReplayPath;
	ret.battle.SetPlayBackFilePath(ReplayPath);
	ret.SpringVersion = LSL::Util::BeforeLast(LSL::Util::AfterLast(FileName, "_"), ".");
	ret.MapName = LSL::Util::BeforeLast(FileName, "_");

	if (!wxFileExists(TowxString(ReplayPath))) {
		wxLogWarning(wxString::Format(_T("File %s does not exist!"), ReplayPath.c_str()));
		MarkBroken(ret);
		return false;
	}
	wxFile replay(TowxString(ReplayPath), wxFile::read);
	if (!replay.IsOpened()) {
		wxLogWarning(wxString::Format(_T("Could not open file %s for reading!"), ReplayPath.c_str()));
		MarkBroken(ret);
		return false;
	}

	//try to get date from filename
	wxDateTime rdate;

	if (rdate.ParseFormat(TowxString(FileName), _T("%Y%m%d_%H%M%S")) == 0) {
		wxLogWarning(wxString::Format(_T("Name of the file %s could not be parsed!"), ReplayPath.c_str()));
		MarkBroken(ret);
		return false;
	}
	ret.date = rdate.GetTicks(); // now it is sorted properly
	ret.date_string = STD_STRING(rdate.FormatISODate() + _T(" ") + rdate.FormatISOTime());

	if (replay.Length() == 0) {
		replay.Close();
		MarkBroken(ret);
		return false;
	}

	const int replay_version = replayVersion(replay);
	ret.battle.SetScript(GetScriptFromReplay(replay, replay_version));

	if (ret.battle.GetScript().empty()) {
		wxLogWarning(wxString::Format(_T("File %s have incompatible version!"), ReplayPath.c_str()));
		MarkBroken(ret);
		return false;
	}

	GetHeaderInfo(replay, ret, replay_version);
	ret.battle.GetBattleFromScript(false);
	ret.GameName = ret.battle.GetHostGameName();
	ret.battle.SetBattleType(BT_Replay);
	ret.battle.SetEngineName("spring");
	ret.battle.SetEngineVersion(ret.SpringVersion);
	ret.battle.SetPlayBackFilePath(ReplayPath);

	return true;
}

std::string ReplayList::GetScriptFromReplay(wxFile& replay, const int version) const
{
	std::string script;
	if (!replay.IsOpened())
		return script;
	if (replay.Seek(20) == wxInvalidOffset) {
		return script;
	}
	int headerSize = 0;
	replay.Read(&headerSize, 4);
	const int seek = 64 + (version < 5 ? 0 : 240);
	if (replay.Seek(seek) == wxInvalidOffset) {
		return script;
	}
	wxFileOffset scriptSize = 0;
	replay.Read(&scriptSize, 4);
	scriptSize = LSL::Util::Clamp(wxFileOffset(scriptSize), wxFileOffset(0), replay.Length());
	if (replay.Seek(headerSize) == wxInvalidOffset)
		return script;
	script.resize(scriptSize, 0);
	replay.Read(&script[0], scriptSize);
	return script;
}

// see https://github.com/spring/spring/blob/develop/rts/System/LoadSave/demofile.h
void ReplayList::GetHeaderInfo(wxFile& replay, StoredGame& rep, const int version) const
{
	if (replay.Seek(304) == wxInvalidOffset) {
		return;
	}
	int gametime = 0;
	replay.Read(&gametime, 4);
	rep.duration = gametime;
	rep.size = replay.Length();
}

