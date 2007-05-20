//
// Class: NickListCtrl
//

#include "nicklistctrl.h"
#include "utils.h"

NickListCtrl::NickListCtrl( wxWindow* parent )
: wxListCtrl( parent, -1, wxDefaultPosition, wxDefaultSize, 
             wxVSCROLL | wxLC_REPORT | wxLC_VIRTUAL | wxLC_NO_HEADER | wxLC_SINGLE_SEL )
{
  wxListItem col;
  col.SetText( _("Nick") );
  InsertColumn( 0, col );
  SetColumnWidth( 0, 160 );
}


NickListCtrl::~NickListCtrl()
{
  
}

void NickListCtrl::UpdateSize()
{
  assert( m_users != NULL );
  SetItemCount( m_users->GetNumUsers() );
}

wxString NickListCtrl::OnGetItemText(long item, long column) const
{
  if ( m_users == NULL ) return _("Error: no list");
  if ( item >= m_users->GetNumUsers() ) return _("Error: index >= max");
  return WX_STRING( m_users->GetUser( item )->GetNick() );
}


int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
  return -1;
}


wxListItemAttr* NickListCtrl::OnGetItemAttr(long item) const
{
  return NULL;
}


