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

namespace libtorrent{ class session; };

class TorrentWrapper
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    void ChangeListeningPort( unsigned int port );

    void ReloadLocalFileList();

    bool RequestFile( const wxString& hash );
  private:

    void CreateTorrent( const wxString& hash, const wxString& name, MediaType type );
    void JoinTorrent( const wxString& name );
    void SocketSend( const wxString& msg );

    void ReceiveandExecute( const wxString& msg );

    wxArrayString m_tracker_urls;
    unsigned int m_open_torrents_number;

    std::vector<TorrentData> m_torrents_infos;
    typedef std::vector<TorrentData>::iterator TorrentsIter;
    std::list<wxString> m_seed_requests;
    typedef std::list<wxString>::iterator SeedReqIter;
    std::vector<TorrentData> m_local_files;
    typedef std::vector<TorrentData>::iterator LocalFilesIter;
    libtorrent::session* torr;
};

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
