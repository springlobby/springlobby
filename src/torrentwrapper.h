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
  unsigned long hash;
  wxString name;
  MediaType type;
};

class libtorrent::session;

class TorrentWrapper
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    void ChangeListeningPort( unsigned int port );

    void JoinTorrent( const wxString& name, unsigned int hash, MediaType type );
  private:

    void CreateTorrent( unsigned int hash, const wxString& name, MediaType type );

    wxArrayString m_tracker_urls;
    unsigned int m_open_torrents_number;

    std::vector<TorrentData> m_torrents_infos;
    typedef std::vector<TorrentData>::iterator TorrentsIter;
    std::list<unsigned int> m_seed_requests;
    typedef std::list<unsigned int>::iterator SeedReqIter;
    std::vector<unsigned int> m_local_files;
    typedef std::vector<unsigned int>::iterator LocalFilesIter;
    libtorrent::session* torr;
}

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
