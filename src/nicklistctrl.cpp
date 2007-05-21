//
// Class: NickListCtrl
//

#include "nicklistctrl.h"
#include "utils.h"

#include "images/bot.xpm"
#include "images/admin.xpm"

NickListCtrl::NickListCtrl( wxWindow* parent )
: wxListCtrl( parent, -1, wxDefaultPosition, wxDefaultSize, 
             wxSUNKEN_BORDER | wxLC_REPORT | wxLC_VIRTUAL | wxLC_NO_HEADER | wxLC_SINGLE_SEL )
{
  wxListItem col;
  col.SetText( _("") );
  InsertColumn( 0, col );
  col.SetText( _("Nick") );
  InsertColumn( 0, col );
  SetColumnWidth( 0, 16 );
  SetColumnWidth( 1, 144 );
  
  m_imagelist = new wxImageList( 16, 16 );
  m_imagelist->Add( wxBITMAP(admin) );
  m_imagelist->Add( wxBITMAP(bot) );
  
  SetImageList( m_imagelist, wxIMAGE_LIST_NORMAL );
  SetImageList( m_imagelist, wxIMAGE_LIST_SMALL );
  SetImageList( m_imagelist, wxIMAGE_LIST_STATE );

}


NickListCtrl::~NickListCtrl()
{
  delete m_imagelist;
}

void NickListCtrl::UpdateSize()
{
  assert( m_users != NULL );
  SetItemCount( m_users->GetNumUsers() );
  //RefreshItems( 0, GetItemCount() );
}

wxString NickListCtrl::OnGetItemText(long item, long column) const
{
  if ( column == 0 ) {
    return _("");
  }
  
  if ( m_users == NULL ) return _("Error: no list");
  if ( item >= m_users->GetNumUsers() ) return _("Error: index >= max");

  return WX_STRING( m_users->GetUser( item ).GetNick() );

}


int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
  std::cout << "** NickListCtrl::OnGetItemColumnImage()" << std::endl;
  //return 1;
  if ( column == 0 ) {
    if ( m_users == NULL ) return -1;
    if ( item >= m_users->GetNumUsers() ) return -1;
    if (m_users->GetUser( item ).GetStatus().bot ) return 1;
    else if (m_users->GetUser( item ).GetStatus().moderator ) return 0;
  }
  return -1;
}


int NickListCtrl::OnGetItemImage(long item) const
{
  std::cout << "** NickListCtrl::OnGetItemImage()" << std::endl;
  
  if ( m_users == NULL ) return -1;
  if ( item >= m_users->GetNumUsers() ) return -1;
  if (m_users->GetUser( item ).GetStatus().bot ) return 1;
  else if (m_users->GetUser( item ).GetStatus().moderator ) return 0;

  return -1;
}

wxListItemAttr* NickListCtrl::OnGetItemAttr(long item) const
{
  return NULL;
}


