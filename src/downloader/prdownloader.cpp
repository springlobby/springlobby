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

#include <list>

PrDownloader::PrDownloader()
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

bool PrDownloader::Get(std::list<IDownloader*> &loaders, const std::string &name, IDownload::category cat)
{
    std::list<IDownload*> results;
    std::list<IDownloader*>::const_iterator it = loaders.begin();
    for( ; it != m_game_loaders.end(); ++it ) {
        (*it)->search(results, name, cat);
        if (results.size()) {
            m_pending_downloads.push( DownloadItem(results, *it));
            return results.size();
        }
    }
    return false;
}

PrDownloader& prDownloader()
{
    static LineInfo<PrDownloader> m( AT );
    static GlobalObjectHolder<PrDownloader, LineInfo<PrDownloader> > s_PrDownloader( m );
    return s_PrDownloader;
}
