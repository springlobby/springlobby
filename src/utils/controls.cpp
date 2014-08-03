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

const wxChar* TooltipEnable(const wxChar* input)
{
	#if !defined(HAVE_WX29) || defined(__WXOSX_COCOA__)
		if (!main_app_has_focus) return wxEmptyString;
		return sett().GetShowTooltips() ? input : wxEmptyString;
    #else
		wxString dummy = wxEmptyString;
		if (!main_app_has_focus) return dummy.wc_str();
		return sett().GetShowTooltips() ? input : dummy.wc_str();
    #endif
}
const wxChar* TooltipEnable(const wxString& input)
{
	//i know this duplicates the above function, but I couldn't figure out a proper conversion from wxString -> wxChar* in 2.9.x
	//and now it's completely useless to branch here?
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
