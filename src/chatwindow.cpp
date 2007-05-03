/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: ChatWindow
//
// Created on: Wed May  2 21:07:18 2007
//

#include "chatwindow.h"


ChatWindow::ChatWindow( wxWindow* parent, bool show_nick_list ) 
: wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize, 0, wxPanelNameStr )
{
  m_show_nick_list = show_nick_list;
 
  SetBackgroundColour( *wxBLUE );
  
  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_chat_sizer = new wxBoxSizer( wxVERTICAL );
  m_say_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_nick_sizer = new wxBoxSizer( wxVERTICAL );
  
  m_chatlog_text = new wxTextCtrl( this, -1, _(""), wxDefaultPosition, wxDefaultSize, 
                             wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );

  m_say_text = new wxTextCtrl( this, -1, _(""), wxDefaultPosition, wxDefaultSize );
  m_say_button = new wxButton( this, -1, _T("Send") );
  
  m_say_sizer->Add( m_say_text, 1, wxEXPAND );
  m_say_sizer->Add( m_say_button );
  
  m_chat_sizer->Add( m_chatlog_text, 1, wxEXPAND );
  m_chat_sizer->Add( m_say_sizer );
  
  m_main_sizer->Add( m_chat_sizer, 1, wxEXPAND );
  m_main_sizer->Add( m_nick_sizer );
  
  
  SetSizer( m_main_sizer );
}


ChatWindow::~ChatWindow()
{
  // TODO: put destructor code here
}


