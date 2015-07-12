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
/*
static const DownloadsObserver& DownloadsObserver::Instance()
{
    static DownloadsObserver downloadsObserver;
    return downloadsObserver;
}
*/
void DownloadsObserver::Add(IDownload* dl)
{
	ContentManager::Instance()->OnDownloadStarted(dl);

	wxMutexLocker lock(mutex);
	m_ActiveDownloadsList.push_back(dl);
}

void DownloadsObserver::Remove(IDownload* dl)
{
	ContentManager::Instance()->OnDownloadFinished(dl);

	wxMutexLocker lock(mutex);
	m_ActiveDownloadsList.remove(dl);

	ObserverDownloadInfo di = GetInfo(dl);
	di.finished = 1;

	m_FinishedDownloadsList.push_back(di);
}

void DownloadsObserver::GetList(std::list<ObserverDownloadInfo>& lst)
{
	wxMutexLocker lock(mutex);
	std::list<IDownload*>::iterator it;
	for (it = m_ActiveDownloadsList.begin(); it != m_ActiveDownloadsList.end(); ++it) {
		ObserverDownloadInfo di = GetInfo((*it));
		if (di.size > 0)
			lst.push_back(di);
	}

	lst.splice(lst.begin(), m_FinishedDownloadsList);
}

void DownloadsObserver::GetMap(std::map<wxString, ObserverDownloadInfo>& map)
{
	wxMutexLocker lock(mutex);
	std::list<IDownload*>::iterator it_dl;
	for (it_dl = m_ActiveDownloadsList.begin(); it_dl != m_ActiveDownloadsList.end(); ++it_dl) {
		ObserverDownloadInfo di = GetInfo((*it_dl));
		if (di.size > 0)
			map[di.name] = di;
	}

	std::list<ObserverDownloadInfo>::iterator it_di;
	for (it_di = m_FinishedDownloadsList.begin(); it_di != m_FinishedDownloadsList.end(); ++it_di) {
		ObserverDownloadInfo di = (*it_di);
		if (di.size > 0)
			map[di.name] = di;
	}
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
	return m_ActiveDownloadsList.empty();
}
