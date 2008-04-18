/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/string.h>

#include "torrentlistctrl.h"
#include "torrentwrapper.h"
#include "utils.h"
#include "user.h"
#include "iconimagelist.h"
#include "battle.h"
#include "uiutils.h"
#include "ui.h"
#include "server.h"
#include "countrycodes.h"

#define TOOLTIP_DELAY 1000

BEGIN_EVENT_TABLE(TorrentListCtrl, customListCtrl)

  EVT_LIST_ITEM_SELECTED   ( BLIST_LIST, TorrentListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( BLIST_LIST, TorrentListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( BLIST_LIST, TorrentListCtrl::OnDeselected )
  EVT_LIST_ITEM_RIGHT_CLICK( BLIST_LIST, TorrentListCtrl::OnListRightClick )
  EVT_LIST_COL_CLICK       ( BLIST_LIST, TorrentListCtrl::OnColClick )
#if wxUSE_TIPWINDOW
#ifndef __WXMSW__ //disables tooltips on win
  EVT_MOTION(TorrentListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()

map_infos* TorrentListCtrl::m_info_map = 0;

TorrentListCtrl::TorrentListCtrl( wxWindow* parent, Ui& ui ):
  customListCtrl(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT),
  m_selected(-1)

{

  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );

//struct TorrentInfos
//{
//  float numnumcopies;
//  wxString name;
//  unsigned int downloaded;
//  unsigned int uploaded;
//  bool leeching;
//  float progress;
//  float inspeed;
//  float outspeed;
//};

  wxListItem col;

  col.SetText( _T("Name") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 0, col, _T("Name"), false );

  col.SetText( _T("numcopies") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 1, col, _T("complete numcopies"), false);

  col.SetText( _T("MB downloaded") );
  col.SetImage(  icons().ICON_NONE);
  InsertColumn( 2, col, _T("MB downloaded"), false );

  col.SetText( _("MB uploaded") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 3, col, _T("MB uploaded") );

  col.SetText( _("leeching") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 4, col, _T("leeching") );

  col.SetText( _("%") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 5, col, _T("% complete") );

  col.SetText( _("KB/s up") );
  col.SetImage( icons().ICON_NONE);
  InsertColumn( 6, col, _T("KB/s upload") );

  col.SetText( _("KB/s down") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 7, col, _T("KB/s download"), false );

  col.SetText( _("ETA") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 8, col, _T("Estimated time of arrival"), false );



  m_sortorder[0].col = 0;
  m_sortorder[0].direction = true;

  //TODO set def sort order
//  m_sortorder[1].col = 5;
//  m_sortorder[1].direction = true;
//  m_sortorder[2].col = 9;
//  m_sortorder[2].direction = true;
//  m_sortorder[3].col = 4;
//  m_sortorder[3].direction = true;
  Sort( );

#ifdef __WXMSW__
  SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 7, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 8, wxLIST_AUTOSIZE_USEHEADER );
#else
  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );

  SetColumnWidth( 7, 28 ); // alittle more than before for dual digets
  SetColumnWidth( 8, 28 );
#endif

  SetColumnWidth( 3, 170 );
  SetColumnWidth( 4, 140 );
  SetColumnWidth( 5, 130 );
  SetColumnWidth( 6, 110 );

//  m_popup = new wxMenu( _T("") );
//  // &m enables shortcout "alt + m" and underlines m
//  m_popup->Append( BLIST_DLMAP, _("Download &map") );
//  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}
void TorrentListCtrl::SetInfoMap( map_infos* map)
{
    m_info_map = map;
}

TorrentListCtrl::~TorrentListCtrl()
{
  delete m_popup;
}


void TorrentListCtrl::OnSelected( wxListEvent& event )
{
  m_selected = GetItemData( event.GetIndex() );
  event.Skip();
}


void TorrentListCtrl::OnDeselected( wxListEvent& event )
{
  if ( m_selected == (int)GetItemData( event.GetIndex() )  )
  m_selected = -1;
}


int TorrentListCtrl::GetSelectedIndex()
{
  return m_selected;
}


void TorrentListCtrl::OnListRightClick( wxListEvent& event )
{
  PopupMenu( m_popup );
}


void TorrentListCtrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( icons().ICON_NONE );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}
  if ( i > 3 ) { i = 3; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  //col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}


void TorrentListCtrl::Sort()
{
  for (int i = 3; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&CompareNameUP:&CompareNameDOWN , 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareCopiesUP:&CompareCopiesDOWN , 0 ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&CompareDownSizeUP:&CompareDownSizeDOWN , 0 ); break;
      case 3 : SortItems( ( m_sortorder[ i ].direction )?&CompareUpSizeUP:&CompareUpSizeDOWN , 0 ); break;
      case 4 : SortItems( ( m_sortorder[ i ].direction )?&CompareLeechUP:&CompareLeechDOWN , 0 ); break;
      case 5 : SortItems( ( m_sortorder[ i ].direction )?&CompareCompletedUP:&CompareCompletedDOWN , 0 ); break;
      case 6 : SortItems( ( m_sortorder[ i ].direction )?&CompareUpSpeedUP:&CompareUpSpeedDOWN , 0 ); break;
      case 7 : SortItems( ( m_sortorder[ i ].direction )?&CompareDownSpeedUP:&CompareDownSpeedDOWN , 0 ); break;
      case 8 : SortItems( ( m_sortorder[ i ].direction )?&CompareEtaUP:&CompareEtaDOWN , 0 ); break;
    }
  }
}


int wxCALLBACK TorrentListCtrl::CompareNameUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.name < info2.name )
    return -1;
  if ( info1.name > info2.name )
    return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareNameDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

   if ( info1.name < info2.name )
    return -1;
  if ( info1.name > info2.name )
    return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSizeUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.downloaded > info2.downloaded )
      return -1;
  if ( info1.downloaded < info2.downloaded )
      return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSizeDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.downloaded < info2.downloaded )
      return 1;
  if ( info1.downloaded > info2.downloaded )
      return -1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareCopiesUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.numcopies < info2.numcopies )
      return -1;
  if ( info1.numcopies > info2.numcopies )
      return 1;

  return 0;
}



int wxCALLBACK TorrentListCtrl::CompareCopiesDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.numcopies < info2.numcopies )
      return 1;
  if ( info1.numcopies > info2.numcopies )
      return -1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSizeUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.uploaded < info2.uploaded )
      return -1;
  if ( info1.uploaded > info2.uploaded )
      return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSizeDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.uploaded <  info2.uploaded )
   {
      return 1;
   }
  if ( info1.uploaded > info2.uploaded )
    {
      return -1;
    }

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareLeechUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if (  info1.leeching <  info2.leeching )
      return -1;
  if (  info1.leeching >  info2.leeching )
      return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareLeechDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if (  info1.leeching <  info2.leeching )
      return 1;
  if (  info1.leeching >  info2.leeching )
      return -1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareCompletedUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if (  info1.progress <  info2.progress )
      return -1;
  if (  info1.progress >  info2.progress )
      return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareCompletedDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if (  info1.progress <  info2.progress )
      return 1;
  if (  info1.progress >  info2.progress )
      return -1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSpeedUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.outspeed < info2.outspeed )
      return -1;
  if ( info1.outspeed > info2.outspeed )
      return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSpeedDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.outspeed < info2.outspeed )
      return 1;
  if ( info1.outspeed > info2.outspeed )
      return -1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSpeedUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.inspeed < info2.inspeed )
      return -1;
  if ( info1.inspeed > info2.inspeed )
      return 1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSpeedDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];

  if ( info1.inspeed < info2.inspeed )
      return 1;
  if ( info1.inspeed > info2.inspeed )
      return -1;

  return 0;
}


int wxCALLBACK TorrentListCtrl::CompareEtaUP(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];
//TODO FIXME
//  if ( info1.GetNumUsers() - info1.inspeed < info2.GetNumUsers() - info2.inspeed )
//      return -1;
//  if ( info1.GetNumUsers() - info1.inspeed > info2.GetNumUsers() - info2.inspeed )
//      return 1;

  return 0;
}



int wxCALLBACK TorrentListCtrl::CompareEtaDOWN(long item1, long item2, long sortData)
{
  map_infos info_map = *m_info_map;
  TorrentInfos& info1 = info_map[item1];
  TorrentInfos& info2 = info_map[item2];
//TODO FIXME
//  if ( info1.GetNumUsers() - info1.inspeed < info2.GetNumUsers() - info2.inspeed )
//      return 1;
//  if ( info1.GetNumUsers() - info1.inspeed > info2.GetNumUsers() - info2.inspeed )
//      return -1;

  return 0;
}

void TorrentListCtrl::OnMouseMotion(wxMouseEvent& event)
{
#if wxUSE_TIPWINDOW
//	wxPoint position = event.GetPosition();
//
//	try{
//		tipTimer.Start(TOOLTIP_DELAY, wxTIMER_ONE_SHOT);
//		int flag = wxLIST_HITTEST_ONITEM;
//		long *ptrSubItem = new long;
//#ifdef HAVE_WX28
//		long item_hit = HitTest(position, flag, ptrSubItem);
//#else
//		long item_hit = HitTest(position, flag);
//#endif
//
//		if (item_hit != wxNOT_FOUND)
//		{
//			long item = GetItemData(item_hit);
//			map_infos info_map = *m_info_map;
//			Battle& battle = ui->GetServer().battles_iter->GetBattle(item);
//			int coloumn = getColoumnFromPosition(position);
//			switch (coloumn)
//			{
//			case 0: // status
//			m_tiptext = icons().GetBattleStatus(battle);
//				break;
//			case 1: // country
//				m_tiptext = GetFlagNameFromCountryCode(battle.GetFounder().GetCountry());
//				break;
//			case 2: // rank_min
//				m_tiptext = m_colinfovec[coloumn].first;
//				break;
//			case 3: // descrp
//				m_tiptext = battle.GetDescription();
//				break;
//			case 4: //map
//				m_tiptext = battle.GetMapName());
//				break;
//			case 5: //mod
//				m_tiptext = battle.GetModName());
//				break;
//			case 6: // host
//				m_tiptext = battle.GetFounder().GetNick();
//				break;
//			case 7: // specs
//				m_tiptext = _T("Spectators:\n");
//				for (unsigned int i = battle.GetNumUsers()-1; i > battle.GetNumUsers() - battle.inspeed-1;--i)
//				{
//					if (i < battle.GetNumUsers()-1)
//						m_tiptext << _T("\n");
//					m_tiptext << battle.GetUser(i).GetNick() ;
//				}
//				break;
//			case 8: // player
//				m_tiptext = _T("Active Players:\n");
//				for (unsigned int i = 0; i < battle.GetNumUsers()-battle.inspeed;++i)
//				{
//					if ( i> 0)
//						m_tiptext << _T("\n");
//					m_tiptext << battle.GetUser(i).GetNick();
//				}
//				break;
//			case 9: //may player
//				m_tiptext = (m_colinfovec[coloumn].first);
//				break;
//
//			default: m_tiptext = _T("");
//				break;
//			}
//		}
//	}
//	catch(...){}
#endif
}


