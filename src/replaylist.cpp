/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include <memory>

#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <wx/datetime.h>
#include <wx/wfstream.h>
#include <wx/mstream.h>
#include <zlib.h>

#include "gui/customdialogs.h"
#include "replaylist.h"
#include "storedgame.h"
#include "utils/conversion.h"
#include <lslutils/globalsmanager.h>

class PlayBackDataReader
{
private:
	wxInputStream* inputStream;
	unsigned char* dataSource;
	size_t dataSourceSize;
	size_t dataSourcePosition;

public:
	PlayBackDataReader()
	{
		inputStream = nullptr;
		dataSource = nullptr;
		dataSourceSize = 0;
		dataSourcePosition = 0;
	}
	explicit PlayBackDataReader(wxInputStream* inputStream) : PlayBackDataReader()
	{
		this->inputStream = inputStream;
	}

	explicit PlayBackDataReader(unsigned char* p, size_t size) : PlayBackDataReader()
	{
		this->dataSource = p;
		this->dataSourceSize = size;
	}

	int Seek(size_t pos) {
		if (inputStream != nullptr) {
			return inputStream->SeekI(pos);
		} else {
			if (pos >= dataSourceSize) {
				return -1;
			}

			dataSourcePosition = pos;
			return pos;
		}
	}

	void Read(void* target, size_t size)
	{
		if (inputStream != nullptr) {
			inputStream->Read(target, size);
			return;
		} else {
			memcpy(target, &dataSource[dataSourcePosition], size);
		}
	}

	size_t GetLength()
	{
		if (inputStream != nullptr) {
			return inputStream->GetLength();
		}

		return dataSourceSize;
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
	wxFileInputStream inputFileStream(ReplayPath);

	if (!inputFileStream) {
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

	if (inputFileStream.GetLength() == 0) {
		MarkBroken(ret);
		return false;
	}

	std::unique_ptr<unsigned char> ptr;
	std::unique_ptr<PlayBackDataReader> replay;

	if (ReplayPath.substr(ReplayPath.length() - 5) == ".sdfz") {
		const size_t CHUNK_SIZE = 20480;
		unsigned char* buf = new unsigned char[CHUNK_SIZE];
		ptr.reset(buf);

		size_t dataSourceSize = getUnzippedData(buf, CHUNK_SIZE, inputFileStream);

		replay.reset(new PlayBackDataReader(buf, dataSourceSize));
	} else {
		replay.reset(new PlayBackDataReader(&inputFileStream));
	}

	const int replay_version = replayVersion(*replay);
	ret.battle.SetScript(GetScriptFromReplay(*replay, replay_version));

	if (ret.battle.GetScript().empty()) {
		wxLogWarning(wxString::Format(_T("File %s have incompatible version!"), ReplayPath.c_str()));
		MarkBroken(ret);
		return false;
	}

	GetHeaderInfo(*replay, ret, replay_version);
	ret.battle.GetBattleFromScript(false);
	ret.GameName = ret.battle.GetHostGameName();
	ret.battle.SetBattleType(BT_Replay);
	ret.battle.SetEngineName("spring");
	ret.battle.SetEngineVersion(ret.SpringVersion);
	ret.battle.SetPlayBackFilePath(ReplayPath);

	return true;
}

size_t ReplayList::getUnzippedData(unsigned char* ptr, size_t bufSize, wxInputStream& inputStream) const
{
	wxASSERT(ptr != nullptr);
	wxASSERT(bufSize > 0);

    z_stream strm = {0};

    std::unique_ptr<unsigned char> inBuff;
    inBuff.reset(new unsigned char[bufSize]);

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = inBuff.get();

    inflateInit2 (& strm, 15 | 32);

    inputStream.Read(inBuff.get(), bufSize);
    strm.avail_in = inputStream.LastRead();
    strm.avail_out = bufSize;
    strm.next_out = ptr;

    inflate (&strm, Z_NO_FLUSH);
    size_t readBytes = bufSize - strm.avail_out;

    inflateEnd (&strm);

    return readBytes;
}

std::string ReplayList::GetScriptFromReplay(PlayBackDataReader& replay, const int version) const
{
	std::string script;
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
	scriptSize = LSL::Util::Clamp(wxFileOffset(scriptSize), wxFileOffset(0), (wxFileOffset)replay.GetLength());
	if (replay.Seek(headerSize) == wxInvalidOffset)
		return script;
	script.resize(scriptSize, 0);
	replay.Read(&script[0], scriptSize);
	return script;
}

// see https://github.com/spring/spring/blob/develop/rts/System/LoadSave/demofile.h
void ReplayList::GetHeaderInfo(PlayBackDataReader& replay, StoredGame& rep, const int version) const
{
	if (replay.Seek(304) == wxInvalidOffset) {
		return;
	}
	int gametime = 0;
	replay.Read(&gametime, 4);
	rep.duration = gametime;
	rep.size = replay.GetLength();
}

