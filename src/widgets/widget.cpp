#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "widget.h"

#include <wx/arrstr.h>
#include <wx/sstream.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/protocol/http.h>
#include <wx/xml/xml.h>

#include "../utils/misc.h"
#include "../utils/conversion.h"
#include "../settings.h"
#include <lslunitsync/unitsync.h>

const int invalid_id = -1;

Widget::Widget():
    w_id(-1),
    n_id(-1),
    num_downloads(-1),
    num_images(0),
    rev_major(-1),
    rev_minor(-1),
    is_installed(false)
{
    //ctor
}

Widget::~Widget()
{
    //dtor
}

bool Widget::IsInstalled() const
{
    return false;
}

wxArrayString Widget::GetImageFilenames() const
{
    wxArrayString ret;
    ExtendedInfo::Images::const_iterator it = extendedinfo.images.begin();
    for ( ; it != extendedinfo.images.end(); ++it ) {
        ret.Add( it->local_path );
    }
    return ret;
}

bool Widget::GetImageInfos()
{
	bool success = false;
	wxHTTP http;

	http.SetTimeout(6);
	http.Connect(widgetDownloader_baseUrl);
	wxString query_url = _T("/lua_manager.php?m=4&id=") + TowxString( n_id );
        // PHP file sending XML content
	wxInputStream *httpStream = http.GetInputStream( query_url );

	if (http.GetError() == wxPROTO_NOERR)
	{
                // will crash here, if xml content is not formatted PERFECTLY
		wxXmlDocument xml(*httpStream);
		wxXmlNode *node = xml.GetRoot() ? xml.GetRoot()->GetChildren() : NULL;
		while (node)
		{
		    int id = FromwxString<long>( node->GetPropVal( _T("ID"), TowxString( invalid_id ) ) );
            if ( id != invalid_id ) {
                WidgetImage file;
                file.id = id;

                wxXmlNode* item = node->GetChildren();
                while( item ) {
                    wxString item_name = item->GetName();
                    if ( item_name == _T("Url") ) {
                        file.url = item->GetNodeContent();
                    }
                    item = item->GetNext();
                }
                extendedinfo.images.push_back( file );
            }

			node = node->GetNext();
			success = true;
		}
	}
	http.Close();
	wxDELETE(httpStream);

	return success;
}

bool Widget::DownloadImages()
{
    wxString sep ( wxFileName::GetPathSeparator() );

    ExtendedInfo::Images& images = extendedinfo.images;
    ExtendedInfo::Images::iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {

        wxString fileurl = it->url;
        fileurl.Replace( _T("http://") , _T("") );
        wxString destpath = sett().GetCachePath() + fileurl.AfterLast(_T('/'));
        tryCreateDirectory(destpath);
        it->local_path = destpath;

        if ( wxFileExists( destpath ) ) // no need to redownload images
            continue;


        wxHTTP FileDownloading;
        /// normal timeout is 10 minutes.. set to 10 secs.
        FileDownloading.SetTimeout(10);
        FileDownloading.Connect( fileurl.BeforeFirst(_T('/')), 80);

        wxInputStream* httpstream = FileDownloading.GetInputStream( _T("/") + fileurl.AfterFirst(_T('/')) );

        if ( httpstream )
        {
            try
            {
              wxFileOutputStream outs( destpath );
              httpstream->Read(outs);
              outs.Close();
              delete httpstream;
              httpstream = 0;
              //download success

            }
            catch (std::exception& e)
            {
                wxLogError(_T("exception on download of %s: %s"), fileurl.c_str(), TowxString(e.what()).c_str());
                return false;
            }
        }
    }
    return true;
}

bool Widget::GetFileInfos()
{
	bool success = false;
    unsigned int file_present_count = 0;
    wxString sep ( wxFileName::GetPathSeparator() );
	wxHTTP http;

	http.SetTimeout(6);
	http.Connect(widgetDownloader_baseUrl);
	wxString query_url = _T("/lua_manager.php?m=1&id=") + TowxString( w_id );
        // PHP file sending XML content
	wxInputStream *httpStream = http.GetInputStream( query_url );
	if (http.GetError() == wxPROTO_NOERR)
	{
                // will crash here, if xml content is not formatted PERFECTLY
		wxXmlDocument xml(*httpStream);

		wxXmlNode *node = xml.GetRoot() ? xml.GetRoot()->GetChildren() : NULL;
		while (node)
		{
		    int id = FromwxString<long>( node->GetPropVal( _T("ID"), TowxString( invalid_id ) ) );
            if ( id != invalid_id ) {
                WidgetFile file;
                file.id = id;

                wxXmlNode* item = node->GetChildren();
                while( item ) {
                    wxString item_name = item->GetName();
                    if ( item_name == _T("Url") ) {
                        file.url = item->GetNodeContent();
                    }
                    else if ( item_name == _T("MD5") ) {
                        file.md5 = item->GetNodeContent();
                    }
                    else if ( item_name == _T("LocalPath") ) {
                        file.local_path = item->GetNodeContent();
                        file_present_count += LSL::usync().FileExists(STD_STRING(file.local_path));
                    }
                    item = item->GetNext();
                }

                extendedinfo.files.push_back( file );
            }

			node = node->GetNext();
			success = true;
		}
	}

	http.Close();
	wxDELETE(httpStream);

    if ( file_present_count == extendedinfo.files.size() )
        is_installed = true;

	return success;
}

bool Widget::Install()
{
    if ( !extendedinfo.parsed )
        return false;

    wxString sep ( wxFileName::GetPathSeparator() );
    wxChar sep_c ( wxFileName::GetPathSeparator() );

    ExtendedInfo::Files& files = extendedinfo.files;
    ExtendedInfo::Files::iterator it = files.begin();
    for ( ; it != files.end(); ++it ) {

        wxString fileurl = it->url;
        fileurl.Replace( _T("http://") , _T("") );
        wxString destpath = sett().GetCurrentUsedDataDir() + it->local_path;

        wxHTTP FileDownloading;
        /// normal timeout is 10 minutes.. set to 10 secs.
        FileDownloading.SetTimeout(10);
        FileDownloading.Connect( fileurl.BeforeFirst(_T('/')), 80);

        wxInputStream* httpstream = FileDownloading.GetInputStream( _T("/") + fileurl.AfterFirst(_T('/')) );

        if ( httpstream )
        {
            try
            {
                const wxString c_path(destpath.BeforeLast( sep_c ));
                tryCreateDirectory( c_path, 0755, wxPATH_MKDIR_FULL );
                wxFileOutputStream outs( destpath );
                httpstream->Read(outs);
                outs.Close();
                delete httpstream;
                httpstream = 0;
                //download success

            }
            catch (...)
            {
                wxLogMessage(_T("exception on download of") + fileurl);
                return false;
            }
        }
    }
    is_installed = true;
    return true;
}

bool Widget::Remove()
{
    if ( !extendedinfo.parsed || !is_installed )
        return false;

    unsigned int file_remove_successes = 0;

    ExtendedInfo::Files& files = extendedinfo.files;
    ExtendedInfo::Files::const_iterator it = files.begin();
    for ( ; it != files.end(); ++it ) {
        wxString destpath = sett().GetCurrentUsedDataDir() + it->local_path;
        file_remove_successes += wxRemoveFile( destpath );
    }

    if ( file_remove_successes == files.size() ) {
        is_installed = false;
        return true;
    }
    if ( file_remove_successes < files.size() && file_remove_successes > 0 ) {
        is_installed = false;
        return false;
    }
    return false;
}

bool Widget::Equals( const Widget& other ) const
{
    return w_id == other.w_id && ( rev.CmpNoCase( other.rev ) == 0 );
}
