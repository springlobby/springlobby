/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "sltipwin.h"

#if wxUSE_TIPWINDOW

BEGIN_EVENT_TABLE(SLTipWindow, wxTipWindow)
EVT_MOUSEWHEEL(SLTipWindow::Cancel)
END_EVENT_TABLE()

SLTipWindow::SLTipWindow(wxWindow* parent, const wxString& text)
    : wxTipWindow(parent, text)
{
}

void SLTipWindow::Cancel(wxMouseEvent& /*unused*/)
{
	wxTipWindow::Close();
}

#endif
