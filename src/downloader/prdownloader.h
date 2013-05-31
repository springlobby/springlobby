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


#ifndef SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H
#define SPRINGLOBBY_HEADERGUARD_PRDOWNLOADER_H

#include <string>
#include <queue>
#include <list>
#include <lslutils/thread.h>

#include "lib/src/Downloader/Download.h"

class IDownloader;

namespace LSL {
    class WorkerThread;
}

namespace P2P {
enum FileStatus
{
    not_stored	= 0, /// file is not on disk and not downloading
    queued		= 1, /// file is not on disk and queued for download
    leeching	= 2, /// file is being downloaded
    paused		= 3, /// file currently not downloading but has valid handle
    complete	= 4  /// file is on disk / dl completed
};
}

struct DownloadInfo{
    P2P::FileStatus downloadstatus;
    std::string name;
    int numcopies;//TODO remove
    double progress;
    double inspeed;
    double eta;
    double filesize;
};

class SearchItem : public LSL::WorkItem {
public:
    SearchItem(std::list<IDownloader*> loaders, std::string name, IDownload::category cat);
    void Run();

private:
    const std::list<IDownloader*> m_loaders;
    const std::string m_name;
    const IDownload::category m_cat;
    int m_result_size;
};

class PrDownloader
{
public:
    PrDownloader();
	~PrDownloader();

	void ClearFinished();
	void UpdateSettings();
    void RemoveTorrentByName( const std::string& name );
    //! returns true if name found and added to dl list
    int GetGame( const std::string& name );
    //! returns true if name found and added to dl list
    int GetMap( const std::string& name );
    //! returns true if name found and added to dl list
    int GetWidget( const std::string& name );
	void SetIngameStatus( bool ingame );

private:
    //! searches given loaders for filename and pushes fitting workitems into dl_thread
    int Get(std::list<IDownloader*> loaders, const std::string& name, IDownload::category cat );
    std::list<IDownloader*> m_game_loaders;
    std::list<IDownloader*> m_map_loaders;
    LSL::WorkerThread* m_dl_thread;

    friend class SearchItem;
};

PrDownloader& prDownloader();

#endif // PRDOWNLOADER_H
