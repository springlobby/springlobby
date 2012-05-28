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


#include "prdownloader.h"

#include "../globalsmanager.h"
#include "lib/src/Downloader/IDownloader.h"

#include <lslutils/thread.h>
#include <list>

class DownloadItem : public LSL::WorkItem {
public:
    DownloadItem( std::list<IDownload*> item, IDownloader* loader)
        :m_item(item), m_loader(loader)
    {}

    void Run()
    {
        m_loader->download( m_item );
        m_loader->freeResult( m_item );
    }

private:
    std::list<IDownload*> m_item;
    IDownloader* m_loader;
};

PrDownloader::PrDownloader()
    : m_dl_thread(new LSL::WorkerThread())
{
    IDownloader::Initialize();
    m_game_loaders.push_back(rapidDownload);
    m_game_loaders.push_back(httpDownload);
    m_game_loaders.push_back(plasmaDownload);
    m_map_loaders.push_back(httpDownload);
    m_map_loaders.push_back(plasmaDownload);
}

PrDownloader::~PrDownloader()
{
    if ( m_dl_thread )
        m_dl_thread->Wait();
    delete m_dl_thread;
    IDownloader::Shutdown();
}

void PrDownloader::ClearFinished()
{
}

void PrDownloader::UpdateSettings()
{
}

void PrDownloader::RemoveTorrentByName(const std::string &/*name*/)
{
}

int PrDownloader::GetWidget(const std::string &name)
{
    return Get(m_map_loaders, name, IDownload::CAT_LUAWIDGETS);
}

int PrDownloader::GetMap(const std::string &name)
{
    return Get(m_map_loaders, name, IDownload::CAT_MAPS);
}

int PrDownloader::GetGame(const std::string &name)
{
    return Get(m_map_loaders, name, IDownload::CAT_MODS);
}

void PrDownloader::SetIngameStatus(bool /*ingame*/)
{
}

int PrDownloader::Get(std::list<IDownloader*> &loaders, const std::string &name, IDownload::category cat)
{
    std::list<IDownload*> results;
    std::list<IDownloader*>::const_iterator it = loaders.begin();
    for( ; it != m_game_loaders.end(); ++it ) {
        (*it)->search(results, name, cat);
        if (results.size()) {
            DownloadItem* dl_item = new DownloadItem(results, *it);
            m_dl_thread->DoWork(dl_item);
            return results.size();
        }
    }
    return 0;
}

PrDownloader& prDownloader()
{
    static LineInfo<PrDownloader> m( AT );
    static GlobalObjectHolder<PrDownloader, LineInfo<PrDownloader> > s_PrDownloader( m );
    return s_PrDownloader;
}
