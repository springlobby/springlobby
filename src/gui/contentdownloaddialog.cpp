/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "contentdownloaddialog.h"
#include "contentsearchresultslistctrl.h"
#include <wx/sizer.h>
ContentDownloadDialog::ContentDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long int style, const wxString& name):  wxDialog(parent, id, title, pos, size, style, name), WindowAttributesPickle( _T("CONTENTDIALOG"), this, wxSize( 670, 400 ))
{
  m_main_sizer = new wxBoxSizer(wxVERTICAL);
  m_search_res_w = new ContentSearchResultsListctrl(this,-1);
  m_main_sizer->Add(m_search_res_w,1,wxALL | wxEXPAND, 3);
  
  SetSizer( m_main_sizer );
  Layout();
}

bool ContentDownloadDialog::Show(bool show)
{
return wxDialog::Show(show);
}

ContentDownloadDialog::~ContentDownloadDialog()
{

}
