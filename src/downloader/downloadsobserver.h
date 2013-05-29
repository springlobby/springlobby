#ifndef DOWNLOADSOBSERVER_H
#define DOWNLOADSOBSERVER_H
#include <list>
#include <wx/thread.h>
#include <wx/string.h>
#include "lib/src/Downloader/Download.h"
#include "lib/src/Downloader/idownloadsobserver.h"

class wxMutex;
class wxString;

struct ObserverDownloadInfo
{
    unsigned int size;
    unsigned int progress;
    wxString name;
    IDownload::category cat;
    bool finished;
};

class DownloadsObserver: public IDownloadsObserver
{
    public:
        DownloadsObserver();
        virtual ~DownloadsObserver();

        virtual void Add(IDownload* dl);
        virtual void Remove(IDownload* dl);

        void GetList(std::list<ObserverDownloadInfo>& lst);
        void ClearFinished();
    private:
        ObserverDownloadInfo GetInfo(IDownload* dl);

        std::list<IDownload*> m_dl_list;
        std::list<ObserverDownloadInfo> finished;

        wxMutex mutex;
};

DownloadsObserver& downloadsObserver();

#endif // DOWNLOADSOBSERVER_H
