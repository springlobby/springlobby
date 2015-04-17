/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "controls.h"
#include "settings.h"
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/tooltip.h>

bool main_app_has_focus;

void UpdateMainAppHasFocus(bool focus)
{
	main_app_has_focus = focus;
	wxToolTip::Enable(sett().GetShowTooltips());
}
