#include "downloadpanel.h"

#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "../utils.h"
#include "downloadlistctrl.h"
#include "infopanel.h"

const int invalid_id = -1;
const unsigned int max_short_desc_length = 50;


BEGIN_EVENT_TABLE( WidgetDownloadPanel , wxScrolledWindow)

//  EVT_LIST_ITEM_ACTIVATED ( BattleListTab::BATTLE_JOIN , BattleListTab::OnListJoin    )
  EVT_LIST_ITEM_SELECTED  ( WidgetDownloadListctrl::WIDGETLISTCTRL_ID, WidgetDownloadPanel::OnSelect )

END_EVENT_TABLE()

WidgetDownloadPanel::WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style)
{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    m_list = new WidgetDownloadListctrl( this, WidgetDownloadListctrl::WIDGETLISTCTRL_ID );
    m_main_sizer->Add( m_list, 1, wxALL | wxEXPAND );
    bool hu = PopulateList();
    m_list->ResetColumnSizes();
    SetSizer( m_main_sizer );
    Layout();
    m_list->SortList( true );
}

WidgetDownloadPanel::~WidgetDownloadPanel()
{
    //dtor
}

void WidgetDownloadPanel::OnSelect( wxListEvent& event )
{
    if ( m_main_sizer->Detach( m_info_panel ) )
        m_info_panel->Destroy();

    m_list->OnSelected( event );

    m_info_panel = new WidgetInfoPanel( m_list->GetSelectedWidget(), this, ID_PANEL );
    m_main_sizer->Add( m_info_panel, 0, wxEXPAND|wxALL, 4 );
    Layout();
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
                // will crash here, if xml content is not formatted PERFECTLY
		wxXmlDocument xml(*httpStream);

		wxXmlNode *node = xml.GetRoot()->GetChildren();
		while (node)
		{
		    int id = s2l( node->GetPropVal( _T("ID"), i2s( invalid_id ) ) );
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
                    else if ( name == _T("Mods") ) {
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
