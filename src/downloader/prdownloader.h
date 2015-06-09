/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H
#define SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H

#include <string>
#include <queue>
#include <list>

#include "lib/src/Downloader/Download.h"
#include "utils/globalevents.h"

class IDownloader;

namespace LSL
{
class WorkerThread;
}
/*
namespace P2P
{
enum FileStatus {
	not_stored	= 0, /// file is not on disk and not downloading
	queued		= 1, /// file is not on disk and queued for download
	leeching	= 2, /// file is being downloaded
	paused		= 3, /// file currently not downloading but has valid handle
	complete	= 4  /// file is on disk / dl completed
};
}


struct DownloadInfo {
	P2P::FileStatus downloadstatus;
	std::string name;
	int numcopies;//TODO remove
	double progress;
	double inspeed;
	double eta;
	double filesize;
};
*/

class PrDownloader : public wxEvtHandler
{
public:
	PrDownloader();
	~PrDownloader();

	void ClearFinished();
	void UpdateSettings();
	void RemoveTorrentByName(const std::string& name);
	//! returns true if name found and added to dl list
	int GetDownload(const std::string& category, const std::string& name);
	bool Download(const std::string& filename, const std::string& url);
	void SetIngameStatus(bool ingame);
	void OnSpringStarted(wxCommandEvent& data);
	void OnSpringTerminated(wxCommandEvent& data);
	bool IsRunning();
	static std::string GetEngineCat();

private:
	//! searches given loaders for filename and pushes fitting workitems into dl_thread
	int Get(std::list<IDownloader*> loaders, const std::string& name, IDownload::category cat);
	std::list<IDownloader*> m_game_loaders;
	std::list<IDownloader*> m_map_loaders;
	LSL::WorkerThread* m_dl_thread;

	friend class SearchItem;
};

PrDownloader& prDownloader();

#endif // PRDOWNLOADER_H
