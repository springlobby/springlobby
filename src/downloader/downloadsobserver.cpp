/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "contentmanager.h"
#include "downloadsobserver.h"

ObserverDownloadInfo::ObserverDownloadInfo()
    : size(0)
    , progress(0)
    , cat(IDownload::category::CAT_NONE)
    , finished(0)
    , name(wxEmptyString)
{
}

ObserverDownloadInfo::ObserverDownloadInfo(IDownload* dl)
    : size(dl->size)
    , cat(dl->cat)
    , finished(0)
    , filename(wxString::FromUTF8(dl->name.c_str()))
    , name(wxString::FromUTF8(dl->origin_name.c_str()))
{
	if (size > 0) {
		progress = dl->getProgress();
	} else {
		progress = 0;
	}
}

DownloadsObserver::DownloadsObserver()
{
}

DownloadsObserver::~DownloadsObserver()
{
}

void DownloadsObserver::Add(IDownload* dl)
{
	wxMutexLocker lock(mutex);

	ContentManager::Instance()->OnDownloadStarted(dl);
	m_DownloadList.push_back(dl);
}

void DownloadsObserver::Remove(IDownload* dl)
{
	wxMutexLocker lock(mutex);
	ContentManager::Instance()->OnDownloadFinished(dl);
	m_DownloadList.remove(dl);
	m_FinishedDownloadsList.push_back(GetInfo(dl));
}

void DownloadsObserver::GetActiveDownloads(std::list<ObserverDownloadInfo>& lst)
{
	wxMutexLocker lock(mutex);
	for (IDownload* dl: m_DownloadList) {
		ObserverDownloadInfo di = GetInfo(dl);
		lst.push_back(di);
	}
}

 bool DownloadsObserver::GetActiveDownloadInfo(const wxString& name, ObserverDownloadInfo& info)
{
	wxMutexLocker lock(mutex);
	for (IDownload* dl: m_DownloadList) {
		ObserverDownloadInfo di = GetInfo(dl);
		if (di.name == name) {
			info = di;
			return true;
		}
	}
	return false;
}

void DownloadsObserver::ClearFinished()
{
	m_FinishedDownloadsList.clear();
}

ObserverDownloadInfo DownloadsObserver::GetInfo(IDownload* dl)
{
	ObserverDownloadInfo di(dl);
	return di;
}

DownloadsObserver& downloadsObserver()
{
	static DownloadsObserver s_DownloadsObserver;
	return s_DownloadsObserver;
}

bool DownloadsObserver::IsEmpty()
{
	return m_DownloadList.empty();
}
