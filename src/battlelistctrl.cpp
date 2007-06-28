//
// Class: BattleListCtrl
//

#include "battlelistctrl.h"
#include "utils.h"



BEGIN_EVENT_TABLE(BattleListCtrl, wxListCtrl)

  EVT_LIST_ITEM_SELECTED   ( BLIST_LIST, BattleListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( BLIST_LIST, BattleListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( BLIST_LIST, BattleListCtrl::OnDeselected )

END_EVENT_TABLE()


BattleListCtrl::BattleListCtrl( wxWindow* parent ) : wxListCtrl(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL), m_selected(-1)
{
  
  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );
  
  wxListItem col;
  
  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _("c") );
  col.SetImage( -1 );
  InsertColumn( 1, col );

  col.SetText( _("r") );
  col.SetImage( -1 );
  InsertColumn( 2, col );
  
  col.SetText( _T("Description") );
  col.SetImage( -1 );
  InsertColumn( 3, col );
  
  col.SetText( _T("Map") );
  col.SetImage( -1 );
  InsertColumn( 4, col );

  col.SetText( _T("Mod") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _T("Host") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _T("s") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _T("p") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  col.SetText( _T("m") );
  col.SetImage( -1 );
  InsertColumn( 9, col );
#ifdef __WXMSW__
  SetColumnWidth( 0, 45 );
#else
  SetColumnWidth( 0, 20 );
#endif
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 170 );
  SetColumnWidth( 4, 140 );
  SetColumnWidth( 5, 130 );
  SetColumnWidth( 6, 110 );
  SetColumnWidth( 7, 26 );
  SetColumnWidth( 8, 26 );
  SetColumnWidth( 9, 26 );
}


BattleListCtrl::~BattleListCtrl()
{

}


void BattleListCtrl::AddBattle( Battle& battle )
{
  int index = InsertItem( 0, IconImageList::GetBattleStatusIcon( battle ) );
  assert( index != -1 );
  SetItemData(index, (wxUIntPtr)&battle );

  UpdateBattle( index );
}


void BattleListCtrl::RemoveBattle( Battle& battle )
{
  int index = GetBattleIndex( battle );
  DeleteItem( index );
}


bool BattleListCtrl::BattleExists( Battle& battle )
{
  return false;
}


void BattleListCtrl::UpdateBattle( Battle& battle )
{
  UpdateBattle( GetBattleIndex( battle ) );
}


void BattleListCtrl::UpdateBattle( const int& index )
{
  assert( index != -1 );
  
  wxListItem item;
  item.SetId( index );
   
  if (!GetItem( item )) assert(false);
    
  Battle& battle = *((Battle*)GetItemData( index ));
  
  SetItemImage( index, IconImageList::GetBattleStatusIcon( battle ) );
  SetItemColumnImage( index, 2, IconImageList::GetRankIcon( battle.opts().rankneeded, false ) );
  SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( battle.GetFounder().GetCountry() ) );
  SetItem( index, 3, WX_STRING(battle.opts().description) );
  SetItem( index, 4, RefineMapname( WX_STRING(battle.opts().mapname) ), battle.IsMapAvailable()?-1:ICON_NEXISTS );
  SetItem( index, 5, RefineModname( WX_STRING(battle.opts().modname) ), battle.IsModAvailable()?-1:ICON_NEXISTS );
  SetItem( index, 6, WX_STRING(battle.opts().founder) );
  SetItem( index, 7, wxString::Format(_("%d"), battle.opts().spectators) );
  SetItem( index, 8, wxString::Format(_("%d"), battle.GetNumUsers() - battle.opts().spectators ) );
  SetItem( index, 9, wxString::Format(_("%d"), battle.opts().maxplayers) );
  
}


int BattleListCtrl::GetBattleIndex( Battle& battle )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    if ( (unsigned long)&battle == GetItemData( i ) ) return i;
  }
  debug_error( "didn't find the battle." );
  return -1;
}



wxString BattleListCtrl::RefineMapname( wxString mapname )
{
  mapname = mapname.SubString(0, mapname.Find( '.', true ) - 1 );
  mapname.Replace(_("_"), _(" ") );
  mapname.Replace(_("-"), _(" ") );
  return mapname;
}


wxString BattleListCtrl::RefineModname( wxString modname )
{
  modname.Replace(_("Absolute Annihilation"), _("AA") );
  modname.Replace(_("Complete Annihilation"), _("CA") );
  modname.Replace(_("Balanced Annihilation"), _("BA") );
  modname.Replace(_("Expand and Exterminate"), _("EAE") );
  modname.Replace(_("War Evolution"), _("WarEv") );
  modname.Replace(_("TinyComm"), _("TC") );
  modname.Replace(_("BETA"), _("b") );
  modname.Replace(_("Public Alpha"), _("pa") );
  modname.Replace(_("Public Beta"), _("pb") );
  modname.Replace(_("Public"), _("p") );
  modname.Replace(_("Alpha"), _("a") );
  modname.Replace(_("Beta"), _("b") );
  return modname;
}


void BattleListCtrl::OnSelected( wxListEvent& event )
{
  m_selected = event.GetIndex();
}


void BattleListCtrl::OnDeselected( wxListEvent& event )
{
  if ( m_selected == event.GetIndex() )
    m_selected = -1;
}


int BattleListCtrl::GetSelectedIndex()
{
  return m_selected;
}


