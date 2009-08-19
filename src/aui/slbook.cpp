/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "slbook.h"
#include "../settings.h"
#include "../chatpanel.h"
#include "../chatpanelmenu.h"

#include <wx/menu.h>

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

END_EVENT_TABLE()

/** @brief RemovePage
  *
  * @todo: document this function
  */
bool SLChatNotebook::RemovePage(size_t page)
{
    return ParentType::RemovePage( page );
}

/** @brief InsertPage
  *
  * @todo: document this function
  */
bool SLChatNotebook::InsertPage(size_t page_idx, ChatPanel* page, const wxString& caption, bool select, const wxBitmap& bitmap)
{
    return ParentType::InsertPage( page_idx, (wxWindow*)page, caption, select, bitmap);
}

/** @brief AddPage
  *
  * @todo: document this function
  */
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

/** @brief DeletePage
  *
  * @todo: document this function
  */
bool SLChatNotebook::DeletePage(size_t page)
{
    return ParentType::DeletePage( page );
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
    pop->Append(0, _("Close"));
    if ( GetPageCount() > 1)
    {
        pop->Append(0, _("Close all"));
        pop->Append(0, _("Close all others"));
    }

    ChatPanel* cur_page = static_cast<ChatPanel*>( GetPage( event.GetSelection() ) );
    ChatPanelMenu* ch_menu = new ChatPanelMenu( cur_page, false );
    pop->AppendSubMenu ( ch_menu->GetMenu() , _( "Channel" ));
    PopupMenu(pop);


}

