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

#include <wx/string.h>

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
    wxString name;
    int numcopies;//TODO remove
    double progress;
    double inspeed;
    double eta;
    double filesize;
};

class PrDownloader
{
public:
    PrDownloader();

void ClearFinished();
void UpdateSettings();
void RemoveTorrentByName( const wxString& name );
void RequestFileByName( const wxString& name );
void SetIngameStatus( bool ingame );
};

PrDownloader& prDownloader();

#endif // PRDOWNLOADER_H
