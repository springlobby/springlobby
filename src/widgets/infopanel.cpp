#include "infopanel.h"

#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/filename.h>

#include "widget.h"
#include "../utils.h"
#include "../settings.h"

const int invalid_id = -1;

WidgetInfoPanel::WidgetInfoPanel( Widget& widget, wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style),
    m_widget( widget )
{
    if ( !m_widget.extendedinfo.parsed ) {
        GetFileInfos();
         GetImageInfos() ;
            DownloadImages();
    }
    m_widget.extendedinfo.parsed = true;

}

WidgetInfoPanel::~WidgetInfoPanel()
{
    //dtor
}
#include "../ui.h"
bool WidgetInfoPanel::GetFileInfos()
{
    bool success = true;
	wxHTTP http;

	http.SetTimeout(6);
	http.Connect(_T("spring.vsync.de"));
    wxString query_url = _T("/luaManager/lua_manager.php?m=1&id=") + TowxString( m_widget.w_id );
        // PHP file sending XML content
	wxInputStream *httpStream = http.GetInputStream( query_url );
	if (http.GetError() == wxPROTO_NOERR)
	{
                // will crash here, if xml content is not formatted PERFECTLY
		wxXmlDocument xml(*httpStream);

		wxXmlNode *node = xml.GetRoot()->GetChildren();
		ExtendedInfo& info = m_widget.extendedinfo;
		while (node)
		{
		    int id = s2l( node->GetPropVal( _T("ID"), i2s( invalid_id ) ) );
            if ( id != invalid_id ) {
                WidgetFile file;
                file.id = id;

                wxXmlNode* item = node->GetChildren();
                while( item ) {
                    wxString name = item->GetName();
                    if ( name == _T("Url") ) {
                        file.url = item->GetNodeContent();
                    }
                    else if ( name == _T("MD5") ) {
                        file.md5 = item->GetNodeContent();
                    }
                    else if ( name == _T("LocalPath") ) {
                        file.local_path = item->GetNodeContent();
                    }
                    item = item->GetNext();
                }

                info.files.push_back( file );
            }

			node = node->GetNext();
		}
	}
	else
		success = false;

	http.Close();
	wxDELETE(httpStream);

	return success;
}
#include <iostream>
bool WidgetInfoPanel::GetImageInfos()
{
    bool success = true;
	wxHTTP http;

	http.SetTimeout(6);
	http.Connect(_T("spring.vsync.de"));
    wxString query_url = _T("/luaManager/lua_manager.php?m=4&id=") + TowxString( m_widget.n_id );
        // PHP file sending XML content
	wxInputStream *httpStream = http.GetInputStream( query_url );

	if (http.GetError() == wxPROTO_NOERR)
	{
                // will crash here, if xml content is not formatted PERFECTLY
		wxXmlDocument xml(*httpStream);
wxString url;
		wxXmlNode *node = xml.GetRoot()->GetChildren();
		ExtendedInfo& info = m_widget.extendedinfo;
		while (node)
		{
		    int id = s2l( node->GetPropVal( _T("ID"), i2s( invalid_id ) ) );
            if ( id != invalid_id ) {
                WidgetImage file;
                file.id = id;

                wxXmlNode* item = node->GetChildren();
                while( item ) {
                    wxString name = item->GetName();
                    if ( name == _T("Url") ) {
                        file.url = item->GetNodeContent();
                    }
                    item = item->GetNext();
                }
                url += _T("&&");
                info.images.push_back( file );
            }

			node = node->GetNext();
		}
	}
	else
		success = false;

	http.Close();
	wxDELETE(httpStream);

	return success;
}

bool WidgetInfoPanel::DownloadImages()
{
    wxString sep ( wxFileName::GetPathSeparator() );

    ExtendedInfo::Images& images = m_widget.extendedinfo.images;
    ExtendedInfo::Images::iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {

        wxString fileurl = it->url;
        fileurl.Replace( _T("http://") , _T("") );
        wxString destpath = sett().GetCachePath() + fileurl.AfterLast(_T('/'));

        it->local_path = destpath;
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
            catch (...)
            {
                wxLogMessage(_T("exception on download of") + fileurl);
                return false;
            }
        }
    }
    return true;

}
