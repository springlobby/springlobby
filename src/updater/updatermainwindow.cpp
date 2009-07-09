/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "updatermainwindow.h"

#include <wx/sizer.h>

#include "../utils/customdialogs.h"
#include "../utils/activitynotice.h"

BEGIN_EVENT_TABLE( UpdaterMainwindow, wxFrame )
    EVT_CLOSE( UpdaterMainwindow::OnClose )
END_EVENT_TABLE()

/** @brief ~UpdaterMainwindow
  *
  * @todo: document this function
  */
 UpdaterMainwindow::~UpdaterMainwindow()
{

}

/** @brief UpdaterMainwindow
  *
  * @todo: document this function
  */
 UpdaterMainwindow::UpdaterMainwindow()
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_activity_panel = new ActivityNoticePanel( this, _T("") );
    m_main_sizer->Add( m_activity_panel, 1, wxEXPAND | wxALL, 0 );
    SetSizer( m_main_sizer );
    Layout();
}

/** @brief OnClose
  *
  * @todo: document this function
  */
void UpdaterMainwindow::OnClose(wxCloseEvent& evt)
{

}

