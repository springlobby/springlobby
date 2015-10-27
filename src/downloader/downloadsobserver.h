/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef DOWNLOADSOBSERVER_H
#define DOWNLOADSOBSERVER_H
#include <list>
#include <map>
#include <wx/thread.h>
#include <wx/string.h>
#include "lib/src/Downloader/Download.h"
#include "lib/src/Downloader/IDownloadsObserver.h"

class wxMutex;
class wxString;

//! Strcuture which contain information about downloading file
class ObserverDownloadInfo
{
public:
	ObserverDownloadInfo();

	//! Size of file in bytes
	int size;

	//! Downloaded part in bytes
	unsigned int progress;

	//! Category of file
	IDownload::category cat;

	//! Is file downloaded
	bool finished;

	//! Name of the file (map_1)
	wxString filename;

	//! Original name (Map 1)
	wxString name;

private:
	//! Get download infromations from IDownload
	ObserverDownloadInfo(IDownload* dl);

	friend class DownloadsObserver;
};

//! DownloadsObserver collect and control information about downloads
//! This class is thread-safe
class DownloadsObserver : public IDownloadsObserver
{
public:
	DownloadsObserver();
	virtual ~DownloadsObserver();

	//! Add information about download
	//! This function called from IDownload::IDownload
	virtual void Add(IDownload* dl);

	//! Move information about download from m_dl_list to m_finished_list
	//! This function called from IDownload::~IDownload
	virtual void Remove(IDownload* dl);

	//! Fill out list with all active downloads
	void GetActiveDownloads(std::list<ObserverDownloadInfo>& lst);

	//! Fill out map with downloads (Key: Original name)
	bool GetActiveDownloadInfo(const wxString& name, ObserverDownloadInfo& info);

	//! Delete all information about finished downloads
	void ClearFinished();

	bool IsEmpty();

private:
	//! Creatre infromation about download
	ObserverDownloadInfo GetInfo(IDownload* dl);

	//! List with downloads which are in process
	std::list<IDownload*> m_DownloadList;

    //! List with finished downloads
	std::list<ObserverDownloadInfo> m_FinishedDownloadsList;

	//! Mutex fir functions Add, Remove, GetList, GetMap
	wxMutex mutex;
};

DownloadsObserver& downloadsObserver();

#endif // DOWNLOADSOBSERVER_H
