/*
 * main.cpp
 * Copyright (C) Spring lobby project 2007 <tc_@linuxmail.org>
 *
  Spring lobby is free software.
  
  You may redistribute it and/or modify it under the terms of the
  GNU General Public License, as published by the Free Software
  Foundation; either version 2 of the License, or (at your option)
  any later version.
  
  main.cc is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with main.cc.  If not, write to:
    The Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor
    Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <wx/wx.h>
#include "mainwindow.h"

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  MainWindow *mw = new MainWindow();

  mw->Show(TRUE);
  return TRUE;
}
