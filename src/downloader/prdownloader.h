/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H
#define SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H

#include <string>
#include <wx/event.h>

#include "lib/src/pr-downloader.h"

class IDownloader;

namespace LSL
{
class WorkerThread;
}

class PrDownloader : public wxEvtHandler
{
public:
	struct DownloadProgress {
		int filesize;
		int downloaded;
	};
	PrDownloader();
	~PrDownloader();

	void ClearFinished();
	void UpdateSettings();
	void RemoveTorrentByName(const std::string& name);
	/**
		download item via pr-downloader
		url can be a rapid tag or an http-url depending on category
		depending on category filename the output filename, hash
		          url            filename
		CAT_MAP   DeltaSiegeDry  ""
		CAT_GAME  ba:stable      ""
		CAT_LOBBY ""             /tmp/lobby.zip
		CAT_HTTP  http://.../f.  /tmp/f.zip
	*/
	void Download(DownloadEnum::Category cat, const std::string& url, const std::string& filename="");
	void SetIngameStatus(bool ingame);
	void OnSpringStarted(wxCommandEvent& data);
	void OnSpringTerminated(wxCommandEvent& data);
	bool IsRunning();
	static void GetProgress(DownloadProgress& progress);

private:
	LSL::WorkerThread* m_dl_thread;

	friend class SearchItem;
};

PrDownloader& prDownloader();

#endif // PRDOWNLOADER_H
