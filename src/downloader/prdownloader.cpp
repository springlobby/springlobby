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
#include "lib/src/pr-downloader.h"

PrDownloader::PrDownloader()
{
	downloadInit();
}

PrDownloader::~PrDownloader()
{
	downloadShutdown();
}

void PrDownloader::ClearFinished()
{
}

void PrDownloader::UpdateSettings()
{
}

void PrDownloader::RemoveTorrentByName(const wxString &name)
{
}

void PrDownloader::RequestFileByName(const wxString &name)
{
}

void PrDownloader::SetIngameStatus(bool ingame)
{
}

PrDownloader& prDownloader()
{
    static LineInfo<PrDownloader> m( AT );
    static GlobalObjectHolder<PrDownloader, LineInfo<PrDownloader> > s_PrDownloader( m );
    return s_PrDownloader;
}
