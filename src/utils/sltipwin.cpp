/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
#include "sltipwin.h"

#if wxUSE_TIPWINDOW

BEGIN_EVENT_TABLE(SLTipWindow, wxTipWindow)
    EVT_MOUSEWHEEL(SLTipWindow::Cancel)
END_EVENT_TABLE()

SLTipWindow::SLTipWindow(wxWindow *parent, const wxString &text)
    : wxTipWindow(parent,text)
{}

void SLTipWindow::Cancel(wxMouseEvent& event)
{
    wxTipWindow::Close();
}

#endif
