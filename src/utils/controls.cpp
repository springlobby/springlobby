/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
#include "controls.h"
#include "settings.h"

const wxChar* TooltipEnable(const wxChar* input)
{
    return sett().GetShowTooltips() ? input : _("");
}
