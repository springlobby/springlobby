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
// Class: NickListCtrl
//
#include "nicklistctrl.h"


NickListCtrl::NickListCtrl( wxWindow* parent )
: wxListCtrl( parent, -1, wxDefaultPosition, wxDefaultSize, 
             wxLC_REPORT | wxLC_VIRTUAL | wxLC_NO_HEADER | wxLC_SINGLE_SEL )
{
  wxListItem col;
  col.SetText( _("Test") );
  InsertColumn( 0, col );
  SetColumnWidth( 0, wxLIST_AUTOSIZE );
}


NickListCtrl::~NickListCtrl()
{
  
}


wxString NickListCtrl::OnGetItemText(long item, long column) const
{
  return _("test");
}


int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
  return -1;
}


wxListItemAttr* NickListCtrl::OnGetItemAttr(long item) const
{
  return NULL;
}


void NickListCtrl::SetUserList( UserList* users )
{
  m_users = users;
}


UserList* NickListCtrl::GetUserList()
{
  return m_users;
}

