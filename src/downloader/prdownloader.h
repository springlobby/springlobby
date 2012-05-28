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

#include "lib/src/Downloader/Download.h"

class IDownloader;

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

struct DownloadItem {
    DownloadItem( std::list<IDownload*> item, IDownloader* loader)
        :m_item(item), m_loader(loader)
    {}
    std::list<IDownload*> m_item;
    IDownloader* m_loader;
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
    bool Get(std::list<IDownloader*>& loaders, const std::string& name, IDownload::category cat );
    std::queue<DownloadItem> m_pending_downloads;
    std::list<IDownloader*> m_game_loaders;
    std::list<IDownloader*> m_map_loaders;
};

PrDownloader& prDownloader();

#endif // PRDOWNLOADER_H
