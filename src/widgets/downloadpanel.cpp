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
#include "infodialog.h"

const int invalid_id = -1;

WidgetDownloadPanel::WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style)
{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    m_list = new WidgetDownloadListctrl( this, -1 );
    m_main_sizer->Add( m_list, 1, wxALL | wxEXPAND );
    bool hu = PopulateList();
    SetSizer( m_main_sizer );
    Layout();
    m_list->RefreshVisibleItems();
    Widget w = m_list->GetDataFromIndex( 0 );
}

WidgetDownloadPanel::~WidgetDownloadPanel()
{
    //dtor
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
                        w.num_images = s2l( item->GetNodeContent() );
                    }
                    else if ( name == _T("Version") ) {
                        wxString rev = item->GetNodeContent();
                        w.rev_major = s2l( rev.BeforeFirst( '.' ) );
                        w.rev_minor = s2l( rev.AfterFirst( '.' ) );
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
                        desc.Replace( _T("\n"), _T(" "), true );
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
