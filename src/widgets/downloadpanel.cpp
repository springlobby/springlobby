#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "downloadpanel.h"

#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include <wx/sizer.h>
#include <wx/splitter.h>

#include "../utils/conversion.h"
#include "downloadlistctrl.h"
#include "infopanel.h"

const int invalid_id = -1;
const unsigned int max_short_desc_length = 50;


BEGIN_EVENT_TABLE( WidgetDownloadPanel , wxScrolledWindow)

    EVT_SPLITTER_SASH_POS_CHANGED( wxID_ANY, WidgetDownloadPanel::OnSashChanged )
    EVT_LIST_ITEM_SELECTED  ( WidgetDownloadListctrl::WIDGETLISTCTRL_ID, WidgetDownloadPanel::OnSelect )

END_EVENT_TABLE()

WidgetDownloadPanel::WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& /*unused*/,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style, _T("widget-dl") )
{
    m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxDefaultSize );

    m_main_sizer = new wxBoxSizer ( wxHORIZONTAL );
    m_list = new WidgetDownloadListctrl( m_splitter, WidgetDownloadListctrl::WIDGETLISTCTRL_ID );
    PopulateList();
    m_list->ResetColumnSizes();

    Widget dummy;
    m_info_panel = new WidgetInfoPanel( dummy, m_splitter, ID_PANEL );
    m_info_panel->Create();

    m_splitter->SplitVertically( m_list, m_info_panel );
    m_main_sizer->Add( m_splitter, 1, wxEXPAND|wxALL, 4 );

    m_splitter->SetMinimumPaneSize( 400 );
    m_splitter->SetSashPosition( sett().GetSashPosition( GetName() ) );

    SetSizer( m_main_sizer );
    Layout();
    m_list->SortList( true );
}

WidgetDownloadPanel::~WidgetDownloadPanel()
{
    sett().SetSashPosition( GetName(), m_splitter->GetSashPosition( ) );
}

void WidgetDownloadPanel::OnSashChanged( wxSplitterEvent& evt )
{
    sett().SetSashPosition( GetName(), evt.GetSashPosition( ) );
    evt.Skip();
}

void WidgetDownloadPanel::OnSelect( wxListEvent& event )
{
    m_list->OnSelected( event );

    m_splitter->Unsplit();
    m_info_panel = new WidgetInfoPanel( m_list->GetSelectedWidget(), m_splitter, ID_PANEL );
    m_splitter->SplitVertically( m_list, m_info_panel );
    m_splitter->SetSashPosition( sett().GetSashPosition( GetName() ) );

    m_info_panel->Create();
    Layout();
}

bool WidgetDownloadPanel::PopulateList()
{
    bool success = true;
	wxHTTP http;

	http.SetTimeout(6);
	http.Connect(_T("spring.vsync.de"));
        // PHP file sending XML content
	wxInputStream *httpStream = http.GetInputStream(_T("/luaManager/lua_manager.php?m=0"));

	if (http.GetError() == wxPROTO_NOERR)
	{
		wxXmlDocument xml(*httpStream);

		wxXmlNode *node = xml.GetRoot()->GetChildren();
		while (node)
		{
		    int id = FromwxString<long>( node->GetPropVal( _T("ID"), TowxString( invalid_id ) ) );
            if ( id != invalid_id ) {
                Widget w;
                w.w_id = id;

                wxXmlNode* item = node->GetChildren();
                while( item ) {
                    wxString name = item->GetName();
                    if ( name == _T("ImageCount") ) {
                        w.num_images = s2l( item->GetNodeContent() );
                    }
                    else if ( name == _T("DownloadCount") ) {
                        w.num_downloads = s2l( item->GetNodeContent() );
                    }
                    else if ( name == _T("Version") ) {
                        wxString rev = item->GetNodeContent();
                        w.rev_major = s2l( rev.BeforeFirst( '.' ) );
                        w.rev_minor = s2l( rev.AfterFirst( '.' ) );
                        w.rev = rev;
                    }
                    else if ( name == _T("NameId") ) {
                        w.n_id = s2l( item->GetNodeContent() );
                    }
                    else if ( name == _T("Changelog") ) {
                        w.changelog = item->GetNodeContent();
                    }
					else if ( name == _T("Mods") ) {// NOT a "Mod" occurence to replace!
                        w.mods = item->GetNodeContent();
                    }
                    else if ( name == _T("Description") ) {
                        wxString desc = item->GetNodeContent();
                        w.description = desc;
                        desc.Replace( _T("\r\n"), _T(" "), true );
                        desc.Replace( _T("\n"), _T(" "), true );
                        desc.Replace( _T("\r"), _T(" "), true );
                        if ( desc.Len() > max_short_desc_length ) {
                            desc = desc.Left(  max_short_desc_length -1 ) + _T("...");
                        }
                        w.short_description = desc;
                    }
                    else if ( name == _T("Author") ) {
                        w.author = item->GetNodeContent();
                    }
                    else if ( name == _T("Entry") ) {
                        w.date = item->GetNodeContent();
                    }
                    else if ( name == _T("Name") ) {
                        w.name = item->GetNodeContent();
                    }

                    item = item->GetNext();
                }

                m_list->AddWidget( w );
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
