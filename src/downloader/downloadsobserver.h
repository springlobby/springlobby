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

class ObserverDownloadInfo
{
    public:
        ObserverDownloadInfo();

        unsigned int size;
        unsigned int progress;
        IDownload::category cat;
        bool finished;
        wxString filename;
        wxString name;
    private:
        ObserverDownloadInfo(IDownload* dl);

    friend class DownloadsObserver;
};

class DownloadsObserver: public IDownloadsObserver
{
    public:
        DownloadsObserver();
        virtual ~DownloadsObserver();

        virtual void Add(IDownload* dl);
        virtual void Remove(IDownload* dl);

        void GetList(std::list<ObserverDownloadInfo>& lst);
        void GetMap(std::map<wxString,ObserverDownloadInfo>& map);

        void ClearFinished();
    private:
        ObserverDownloadInfo GetInfo(IDownload* dl);

        std::list<IDownload*> m_dl_list;
        std::list<ObserverDownloadInfo> m_finished_list;

        wxMutex mutex;
};

DownloadsObserver& downloadsObserver();

#endif // DOWNLOADSOBSERVER_H
