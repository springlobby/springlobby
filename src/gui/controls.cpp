/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "controls.h"
#include "settings.h"
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/tooltip.h>

bool main_app_has_focus;

void UpdateMainAppHasFocus( bool focus )
{
	main_app_has_focus = focus;
	wxToolTip::Enable(sett().GetShowTooltips()&&main_app_has_focus);
}

const wxChar* TooltipEnable(const wxString& input)
{
#if !defined(HAVE_WX29) || defined(__WXOSX_COCOA__)
	wxString dummy = wxEmptyString;
	if (!main_app_has_focus) return dummy.wc_str();
	return sett().GetShowTooltips() ? input.wc_str() : dummy.wc_str();
#else
	wxString dummy = wxEmptyString;
	if (!main_app_has_focus) return dummy.wc_str();
	return sett().GetShowTooltips() ? input.wc_str() : dummy.wc_str();
#endif
}
