/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "slbook.h"

SLNotebook ::SLNotebook (wxWindow* parent, wxWindowID id , const wxPoint& pos , const wxSize& size , long style )
    : wxAuiNotebook( parent, id, pos, size, style )
{
    m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING |

                            wxAUI_MGR_HINT_FADE |
                             wxAUI_MGR_VENETIAN_BLINDS_HINT
        );
}

