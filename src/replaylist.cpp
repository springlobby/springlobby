/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "replaylist.h"

#include <lslutils/conversion.h>
#include <lslutils/globalsmanager.h>
#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <zlib.h>
#include <memory>

#include "storedgame.h"
#include "utils/conversion.h"

class PlayBackDataReader
{
private:
	wxInputStream* inputStream;
	gzFile gzInputStream;
	std::string m_name;

public:
	PlayBackDataReader()
	    : inputStream(nullptr)
	    , gzInputStream(nullptr)
	{
	}

	~PlayBackDataReader()
	{
		gzclose(gzInputStream);
		gzInputStream = nullptr;

		delete inputStream;
		inputStream = nullptr;
	}

	explicit PlayBackDataReader(const std::string& name)
	    : PlayBackDataReader()
	{
		m_name = name;
		if (name.substr(name.length() - 5) == ".sdfz") {
			gzInputStream = gzopen(name.c_str(), "rb");
			if (gzInputStream == nullptr) {
				wxLogWarning("Couldn't open %s", name.c_str());
				return;
			}
		} else {
			inputStream = new wxFileInputStream(name);
		}
	}

	int Seek(size_t pos)
	{
		if (inputStream != nullptr) {
			return inputStream->SeekI(pos);
		} else {
			return gzseek(gzInputStream, pos, SEEK_SET);
		}
	}

	size_t Read(void* target, size_t size)
	{
		wxASSERT(size > 0);
		if (inputStream != nullptr) {
			inputStream->Read(target, size);
			return inputStream->LastRead();
		} else {
			const int res = gzread(gzInputStream, target, size);
			if (res <= 0) {
				wxLogWarning("Couldn't read from %s", m_name.c_str());
			}
			return res;
		}
	}

	template <typename T>
	bool Get(T& target, const size_t offset)
	{
		if (Seek(offset) == wxInvalidOffset) {
			return false;
		}
		size_t numRead = Read(&target, sizeof(T));
		return sizeof(T) == numRead;
	}

	const std::string& GetName() const
	{
		return m_name;
	}

	bool IsOk() const
	{
		return gzInputStream != nullptr || inputStream != nullptr;
	}
};

IPlaybackList& replaylist()
{
	static LSL::Util::LineInfo<ReplayList> m(AT);
	static LSL::Util::GlobalObjectHolder<ReplayList, LSL::Util::LineInfo<ReplayList> > m_replay_list(m);
	return m_replay_list;
}

ReplayList::ReplayList()
{
}


int ReplayList::replayVersion(PlayBackDataReader& replay) const
{
	int version;
	if (!replay.Get(version, 16))
		version = 0;
	return version;
}

static void MarkBroken(StoredGame& ret)
{
	ret.battle.SetHostMap("broken", "");
	ret.battle.SetHostGame("broken", "");
}

static void FixSpringVersion(std::string& springVersion)
{
	const int version = LSL::Util::FromIntString(springVersion);
	if (LSL::Util::ToIntString(version) != springVersion)
		return;

	springVersion += ".0";
}

bool ReplayList::GetReplayInfos(const std::string& ReplayPath, StoredGame& ret) const
{
	const std::string FileName = LSL::Util::AfterLast(ReplayPath, SEP); // strips file path
	ret.type = StoredGame::REPLAY;
	ret.battle.SetPlayBackFilePath(ReplayPath);
	std::string engineVersion = LSL::Util::BeforeLast(LSL::Util::AfterLast(FileName, "_"), ".");
	ret.size = wxFileName::GetSize(ReplayPath).GetLo(); //FIXME: use longlong

	FixSpringVersion(engineVersion);

	if (!wxFileExists(TowxString(ReplayPath))) {
		wxLogWarning(wxString::Format(_T("File %s does not exist!"), ReplayPath.c_str()));
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

	if (ret.size <= 0) {
		MarkBroken(ret);
		return false;
	}

	PlayBackDataReader* replay = new PlayBackDataReader(ReplayPath);

	if (!replay->IsOk()) {
		wxLogWarning(wxString::Format(_T("Could not open file %s for reading!"), ReplayPath.c_str()));
		MarkBroken(ret);
		delete replay;
		return false;
	}


	const int replay_version = replayVersion(*replay);
	ret.battle.SetScript(GetScriptFromReplay(*replay, replay_version));

	if (ret.battle.GetScript().empty()) {
		wxLogWarning(wxString::Format(_T("File %s have incompatible version!"), ReplayPath.c_str()));
		MarkBroken(ret);
		delete replay;
		return false;
	}

	GetHeaderInfo(*replay, ret, replay_version);
	ret.battle.GetBattleFromScript(false);
	ret.battle.SetBattleType(BT_Replay);
	ret.battle.SetEngineName("spring");
	ret.battle.SetEngineVersion(engineVersion);
	ret.battle.SetPlayBackFilePath(ReplayPath);
	delete replay;
	return true;
}

std::string ReplayList::GetScriptFromReplay(PlayBackDataReader& replay, const int version) const
{
	std::string script;
	int headerSize = 0;
	if (!replay.Get(headerSize, 20))
		return script;

	const int seek = 64 + (version < 5 ? 0 : 240);

	int scriptSizeInt = 0;
	if (!replay.Get(scriptSizeInt, seek)) {
		wxLogWarning("Couldn't seek to scriptsize from demo: %s", replay.GetName().c_str());
		return script;
	}
	wxFileOffset scriptSize = static_cast<wxFileOffset>(scriptSizeInt);
	if (scriptSize <= 0) {
		wxLogWarning("Demo contains empty script: %s (%d)", replay.GetName().c_str(), (int)scriptSize);
		return script;
	}

	if (replay.Seek(headerSize) == wxInvalidOffset) {
		wxLogWarning("Couldn't seek to script from demo: %s (%d)", replay.GetName().c_str());
		return script;
	}
	script.resize(scriptSize, 0);
	replay.Read(&script[0], scriptSize);
	return script;
}

// see https://github.com/spring/spring/blob/develop/rts/System/LoadSave/demofile.h
void ReplayList::GetHeaderInfo(PlayBackDataReader& replay, StoredGame& rep, const int /*version*/) const
{
	if (!replay.Get(rep.duration, 312))
		rep.duration = 0;
}
