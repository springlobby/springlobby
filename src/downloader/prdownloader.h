/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H
#define SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H

#include <string>
#include <wx/event.h>

#include "lib/src/Downloader/DownloadEnum.h"

class IDownloader;

namespace LSL
{
class WorkerThread;
}

class DownloadItem;

class IDownloadItemListener
{
public:
	virtual ~IDownloadItemListener(){}

	virtual void DownloadStarted(const DownloadItem* item) = 0;
	virtual void DownloadFailed(const DownloadItem* item) = 0;
	virtual void DownloadFinished(const DownloadItem* item) = 0;
};

class PrDownloader : public wxEvtHandler, public IDownloadItemListener
{
public:
	struct DownloadProgress {
		DownloadProgress():
			filesize(0),
			downloaded(0)
		{}
		std::string name;
		int filesize;
		int downloaded;
		bool IsFinished() const
		{
			return downloaded >= filesize;
		}
		float GetProgressPercent()
		{
			if (filesize == 0)
				return 0;
			return (float)(downloaded * 100.0)/filesize;
		}
		void CopyTo(DownloadProgress& prg)
		{
			prg.downloaded = downloaded;
			prg.filesize = filesize;
			prg.name = name;
		}
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
	void Download(DownloadEnum::Category cat, const std::string& filename, const std::string& url="");
	void DownloadMap(const std::string& filename);
	void DownloadGame(const std::string& filename);
	void DownloadEngine(const std::string& filename);

	void SetIngameStatus(bool ingame);
	void OnSpringStarted(wxCommandEvent& data);
	void OnSpringTerminated(wxCommandEvent& data);
	bool IsRunning();
	static void GetProgress(DownloadProgress& progress);
	void UpdateApplication(const std::string& updateurl);

	//IDownloadItemListener
	void DownloadStarted(const DownloadItem* item) override;
	void DownloadFailed(const DownloadItem* item) override;
	void DownloadFinished(const DownloadItem* item) override;

private:
	LSL::WorkerThread* m_dl_thread;

	friend class SearchItem;
};

PrDownloader& prDownloader();

#endif // PRDOWNLOADER_H
