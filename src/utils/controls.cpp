/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
#include "controls.h"
#include "../settings.h"
#include "../defines.h"
#include <wx/gdicmn.h>
#include <wx/window.h>

const wxChar* TooltipEnable(const wxChar* input)
{
    #ifndef HAVE_WX29
        return sett().GetShowTooltips() ? input : _("");
    #else
        return sett().GetShowTooltips() ? input : _("").wc_str();
    #endif
}

int GetMaxStringWidth( const wxWindow& win, const wxArrayString& strings )
{
    int max = -1;
    for (size_t i = 0; i < strings.Count(); ++i ) {
        wxSize size;
        win.GetTextExtent( strings[i], &size.x, &size.y );
        max = std::max( max, size.x );
    }

    return max;
}
