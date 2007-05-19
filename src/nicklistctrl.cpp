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


