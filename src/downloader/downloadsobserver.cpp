#include "downloadsobserver.h"

ObserverDownloadInfo::ObserverDownloadInfo():
	size(0),
	progress(0),
	cat(IDownload::category::CAT_NONE),
	finished(0),
	name(wxEmptyString)
{

}

ObserverDownloadInfo::ObserverDownloadInfo(IDownload* dl): finished(0)
{
    filename=wxString::FromUTF8(dl->name.c_str());
    name=wxString::FromUTF8(dl->origin_name.c_str());
    //name=name.AfterLast('/');
    size=dl->size;
    if(size>0)
        progress=dl->getProgress();
    else
        progress=0;
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
    wxMutexLocker lock(mutex);
    m_dl_list.push_back(dl);
}

void DownloadsObserver::Remove(IDownload* dl)
{
    wxMutexLocker lock(mutex);
    m_dl_list.remove(dl);
    ObserverDownloadInfo di=GetInfo(dl);
    di.finished=1;
    m_finished_list.push_back(di);
}

void DownloadsObserver::GetList(std::list<ObserverDownloadInfo>& lst)
{
    wxMutexLocker lock(mutex);
    std::list<IDownload*>::iterator it;
    for(it=m_dl_list.begin();it!=m_dl_list.end();++it)
    {
        ObserverDownloadInfo di=GetInfo((*it));
        if(di.size>0)
            lst.push_back(di);
    }

    lst.splice(lst.begin(),m_finished_list);
}

void DownloadsObserver::GetMap(std::map<wxString,ObserverDownloadInfo>& map)
{
    wxMutexLocker lock(mutex);
    std::list<IDownload*>::iterator it_dl;
    for(it_dl=m_dl_list.begin();it_dl!=m_dl_list.end();++it_dl)
    {
        ObserverDownloadInfo di=GetInfo((*it_dl));
        if(di.size>0)
            map[di.name]=di;
    }

    std::list<ObserverDownloadInfo>::iterator it_di;
    for(it_di=m_finished_list.begin();it_di!=m_finished_list.end();++it_di)
    {
        ObserverDownloadInfo di=(*it_di);
        if(di.size>0)
            map[di.name]=di;
    }

}

void DownloadsObserver::ClearFinished()
{
    m_finished_list.clear();
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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

