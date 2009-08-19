/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "slbook.h"
#include "../settings.h"
#include "../chatpanel.h"
#include "../chatpanelmenu.h"
#include "../ui.h"
#include "../mainwindow.h"

#include <wx/menu.h>

static const long ID_CLOSE_TAB          = wxNewId();
static const long ID_CLOSE_TAB_OTHER    = wxNewId();
static const long ID_CLOSE_TAB_ALL      = wxNewId();
static const long ID_NEW_TAB      = wxNewId();

SLNotebook ::SLNotebook (wxWindow* parent, wxWindowID id , const wxPoint& pos , const wxSize& size , long style )
    : wxAuiNotebook( parent, id, pos, size, sett().GetShowXallTabs() ? style | wxAUI_NB_CLOSE_ON_ALL_TABS : style )
{
    m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING |

                            wxAUI_MGR_HINT_FADE |
                             wxAUI_MGR_VENETIAN_BLINDS_HINT
        );
}

BEGIN_EVENT_TABLE( SLChatNotebook, SLNotebook )

     EVT_AUINOTEBOOK_TAB_RIGHT_UP ( wxID_ANY, SLChatNotebook::OnHeaderRightClick )
//     EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, SLChatNotebook::OnAllowNotebookDnD)

END_EVENT_TABLE()

bool SLChatNotebook::AddPage(ChatPanel* page, const wxString& caption, bool select, const wxBitmap& bitmap)
{
    return ParentType::AddPage((wxWindow*)page, caption, select, bitmap);
}

/** @brief SLChatNotebook
  *
  * @todo: document this function
  */
 SLChatNotebook::SLChatNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : ParentType( parent, id, pos,  size, style )
{

}

//should work accroding to docs/sample, but doesn't (not even in sample)
void SLChatNotebook::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
    evt.Veto();
    evt.Skip();
}

/** @brief OnHeaderRightClick
  *
  * @todo: document this function
  */
void SLChatNotebook::OnHeaderRightClick(wxAuiNotebookEvent &event)
{
    if ( event.GetSelection() == -1 )
        return;

    wxMenu* pop = new wxMenu;
    pop->Append( ID_NEW_TAB, _("New Tab") );
    pop->Append( ID_CLOSE_TAB, _("Close") );
    if ( GetPageCount() > 1)
    {
        pop->Append( ID_CLOSE_TAB_ALL, _("Close all"));
        pop->Append( ID_CLOSE_TAB_OTHER, _("Close all others"));
    }

    ChatPanel* cur_page = static_cast<ChatPanel*>( GetPage( event.GetSelection() ) );
    ChatPanelMenu* ch_menu = new ChatPanelMenu( cur_page, true );
    pop->AppendSubMenu ( ch_menu->GetMenu() , _( "Channel" ));
    Connect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SLChatNotebook::OnMenuItem ), cur_page, this );
    PopupMenu(pop);
}

void SLChatNotebook::OnMenuItem( wxCommandEvent& event )
{
    long id = event.GetId();
    if ( id == ID_NEW_TAB ) {
        ui().mw().OnMenuJoin( event );
    }
    else if ( id == ID_CLOSE_TAB ) {
        DeletePage( GetSelection() );
    }
    else if ( id == ID_CLOSE_TAB_ALL ) {
        for ( size_t i = 0; i < GetPageCount(); ++i ){
                DeletePage( i );
                i--;
        }
    }
    else if ( id == ID_CLOSE_TAB_OTHER ) {
        size_t selected = GetSelection();
        for ( size_t i = 0; i < GetPageCount(); ++i ){
        	if ( i == selected )
        	    continue;
            else {
                DeletePage( i );
                i--;
                selected--;
        	}
        }
    }
    else {
        ChatPanel* cur_page = static_cast<ChatPanel*>( GetPage( GetSelection() ) );
        if ( cur_page ) {
            ChatPanelMenu* ch_menu = new ChatPanelMenu( cur_page, true );
            ch_menu->OnMenuItem( event );
            delete ch_menu;
        }
    }
}
