/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
#include "controls.h"
#include "../settings.h"
#include "../defines.h"

const wxChar* TooltipEnable(const wxChar* input)
{
    #ifndef HAVE_WX29
        return sett().GetShowTooltips() ? input : _("");
    #else
        return sett().GetShowTooltips() ? input : _("").wc_str();
    #endif
}
