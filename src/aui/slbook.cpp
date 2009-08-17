/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "slbook.h"
#include "../settings.h"


SLNotebook ::SLNotebook (wxWindow* parent, wxWindowID id , const wxPoint& pos , const wxSize& size , long style )
    : wxAuiNotebook( parent, id, pos, size, sett().GetShowXallTabs() ? style | wxAUI_NB_CLOSE_ON_ALL_TABS : style )
{
    m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING |

                            wxAUI_MGR_HINT_FADE |
                             wxAUI_MGR_VENETIAN_BLINDS_HINT
        );
}

