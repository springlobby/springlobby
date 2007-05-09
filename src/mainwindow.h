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
// Created on: Tue May  1 11:55:40 2007
//

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <wx/wx.h>
#include <wx/listbook.h>

#include "mainchattab.h"

#include "images/chat_icon.xpm"

class MainWindow : public wxFrame
{
  public:
    MainWindow();
    virtual ~MainWindow();
  
    // MainWindow interface
  
    // TODO: add member function declarations...
  
  protected:
    // MainWindow variables
  
    wxBoxSizer* m_main_sizer;
    wxListbook* m_func_tabs;
    wxNotebookPage* m_chat_page;
  
    MainChatTab* m_chat_tab;
  
    wxImageList* m_func_tab_images;
};


#endif  //_MAINWINDOW_H_

