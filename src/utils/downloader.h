#ifndef DOWNLOADER_HH_INCLUDED
#define DOWNLOADER_HH_INCLUDED


class wxArrayString;
class wxString;
wxArrayString getDownloadLinks( const wxString& name );
void downloadFile( const wxString& host, const wxString& remote_path, const wxString& local_dest );
#endif
