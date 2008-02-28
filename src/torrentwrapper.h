#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <vector>
#include <list>

enum MediaType
{
  map,
  mod
};

struct TorrentData
{
  wxString hash;
  wxString name;
  MediaType type;
  wxArrayString seedurls;
};

class libtorrent::session;

class TorrentWrapper
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    void ChangeListeningPort( unsigned int port );

    void ReloadLocalFileList();

    bool TorrentWrapper::RequestFile( const wxString& hash, MediaType type )
  private:

    void CreateTorrent( unsigned int hash, const wxString& name, MediaType type );
    void JoinTorrent( const wxString& name );

    wxArrayString m_tracker_urls;
    unsigned int m_open_torrents_number;

    std::vector<TorrentData> m_torrents_infos;
    typedef std::vector<TorrentData>::iterator TorrentsIter;
    std::list<wxString> m_seed_requests;
    typedef std::list<wxString>::iterator SeedReqIter;
    std::vector<TorrentData> m_local_files;
    typedef std::vector<TorrentData>::iterator LocalFilesIter;
    libtorrent::session* torr;
}

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
