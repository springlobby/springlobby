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
// Class: MainWindow
//
// Created on: Tue May  1 11:55:40 2007
//

#include "mainwindow.h"


MainWindow::MainWindow() : wxFrame((wxFrame *)NULL, -1, _T("Spring Lobby"),
                               wxPoint(50, 50), wxSize(450, 340))
{
  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_func_tabs = new wxListbook( this, -1, wxDefaultPosition, wxDefaultSize, wxLB_LEFT );
  
  m_func_tab_images = new wxImageList( 64, 64 );
  m_func_tab_images->Add( wxBITMAP(chat_icon) );
  
  m_func_tabs->AssignImageList( m_func_tab_images );
  
  m_chat_tab = new MainChatTab( m_func_tabs );
  m_func_tabs->AddPage( m_chat_tab, _(""), true, 0 );
  //m_func_tabs->AddPage( m_chat_tab, _(""), false, 0 );
  
  m_main_sizer->Add( m_func_tabs, 1, wxEXPAND, 0 );
  
  SetSizer( m_main_sizer );
}


MainWindow::~MainWindow()
{
  // TODO: put destructor code here
}


