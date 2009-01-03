#include "infopanel.h"

#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/filename.h>

#include "widget.h"
#include "../utils.h"
#include "../settings.h"
#include "../ui.h"
#include "../Helper/imageviewer.h"


const int invalid_id = -1;


BEGIN_EVENT_TABLE( WidgetInfoPanel, wxPanel)
    EVT_BUTTON( WidgetInfoPanel::BUT_CHG_LOG, WidgetInfoPanel::OnChangeLog )
    EVT_BUTTON( WidgetInfoPanel::BUT_DOWNLOAD, WidgetInfoPanel::OnDownload )
    EVT_BUTTON( WidgetInfoPanel::BUT_REMOVE, WidgetInfoPanel::OnRemove )
    EVT_BUTTON( WidgetInfoPanel::BUT_UPDATE, WidgetInfoPanel::OnUpdate )
    EVT_BUTTON( WidgetInfoPanel::BUT_PICS, WidgetInfoPanel::OnPics )

END_EVENT_TABLE()

WidgetInfoPanel::WidgetInfoPanel( Widget& widget, wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style),
    m_widget( widget )
{
    m_busy_notice = new wxBoxSizer( wxVERTICAL );
    wxStaticText* m_busy_notice_lbl = new wxStaticText( this, -1, _("getting infos") );
    m_busy_notice->Add( m_busy_notice_lbl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
    SetSizer( m_busy_notice );

}

void WidgetInfoPanel::Create()
{
    if ( !m_widget.extendedinfo.parsed ) {
        GetFileInfos();
         GetImageInfos() ;
            DownloadImages();
    }
    m_widget.extendedinfo.parsed = true;
    m_busy_notice->DeleteWindows();
    m_busy_notice->Show( false );
//    m_busy_notice->Destroy();
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_top_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_grid_sizer = new wxGridSizer( 2, 5, 5 );
    wxStaticText* name_lbl = new wxStaticText( this, -1, _("Name") );
    m_grid_sizer->Add( name_lbl );
    wxStaticText* name = new wxStaticText( this, -1, m_widget.name );
    m_grid_sizer->Add( name );

    wxStaticText* author_lbl = new wxStaticText( this, -1, _("Author") );
    m_grid_sizer->Add( author_lbl );
    wxStaticText* author = new wxStaticText( this, -1, m_widget.author );
    m_grid_sizer->Add( author );

    wxStaticText* mods_lbl = new wxStaticText( this, -1, _("Suitable mods") );
    m_grid_sizer->Add( mods_lbl );
    wxStaticText* mods = new wxStaticText( this, -1, m_widget.mods );
    m_grid_sizer->Add( mods );

    wxStaticText* rev_lbl = new wxStaticText( this, -1, _("Current version") );
    m_grid_sizer->Add( rev_lbl );
    wxStaticText* rev = new wxStaticText( this, -1, m_widget.rev );
    m_grid_sizer->Add( rev );

    wxStaticText* dl_lbl = new wxStaticText( this, -1, _("# downloaded") );
    m_grid_sizer->Add( dl_lbl );
    wxStaticText* dl = new wxStaticText( this, -1, TowxString( m_widget.num_downloads ) );
    m_grid_sizer->Add( dl );

    wxStaticText* publ_lbl = new wxStaticText( this, -1, _("Published on") );
    m_grid_sizer->Add( publ_lbl );
    wxStaticText* publ = new wxStaticText( this, -1, m_widget.date );
    m_grid_sizer->Add( publ );

    m_top_sizer->Add( m_grid_sizer, 1, wxEXPAND, 0 );

    wxStaticBoxSizer* desc_frame = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Description") ), wxVERTICAL );
    wxStaticText* desc = new wxStaticText( this, -1, m_widget.description );
    desc_frame->Add( desc, 1, wxEXPAND| wxALL, 3 );
    m_top_sizer->Add( desc_frame, 1, wxEXPAND|wxLEFT, 10 );

    m_main_sizer->Add( m_top_sizer, 0, wxALL, 5 );

    m_download = new wxButton( this, BUT_DOWNLOAD, _("Download") );
    m_chg_log = new wxButton( this, BUT_CHG_LOG, _("View changelog") );
    m_pics = new wxButton( this, BUT_PICS, _("View Screenshots") );
    m_update = new wxButton( this, BUT_UPDATE, _("Update") );
    m_remove = new wxButton( this, BUT_REMOVE, _("Remove") );

    const int flag = wxALL;
    const int spc = 5;
    m_button_sizer->Add( m_download, 0, flag, spc );
    m_button_sizer->Add( m_update, 0, flag, spc );
    m_button_sizer->Add( m_remove, 0, flag, spc );
    m_button_sizer->Add( m_chg_log, 0, flag, spc );
    m_button_sizer->Add( m_pics, 0, flag, spc );

    m_main_sizer->Add( m_button_sizer, 0, wxEXPAND|wxALL, 0 );
    SetButtonStates();

    SetSizer( m_main_sizer );
    Layout();
}

void WidgetInfoPanel::SetButtonStates()
{
    m_download->Enable( true );
    m_chg_log->Enable( m_widget.changelog != _T("") );
    m_pics->Enable( m_widget.extendedinfo.images.size() > 0 );
    m_update->Enable( false );
    m_remove->Enable( false );

}

WidgetInfoPanel::~WidgetInfoPanel()
{
    //dtor
}

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

void WidgetInfoPanel::OnDownload( wxCommandEvent& evt )
{

}

void WidgetInfoPanel::OnPics( wxCommandEvent& evt )
{
    ImageViewer* iv = new ImageViewer( m_widget.GetImageFilenames(), false, this, -1);
    iv->Show( true );
}

void WidgetInfoPanel::OnChangeLog( wxCommandEvent& evt )
{

}

void WidgetInfoPanel::OnRemove( wxCommandEvent& evt )
{

}

void WidgetInfoPanel::OnUpdate( wxCommandEvent& evt )
{

}

