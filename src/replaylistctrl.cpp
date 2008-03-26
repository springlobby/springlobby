/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>

#include "replaylistctrl.h"
#include "utils.h"
#include "user.h"
#include "iconimagelist.h"
#include "uiutils.h"
#include "ui.h"

//#include "countrycodes.h"

#define TOOLTIP_DELAY 1000

BEGIN_EVENT_TABLE(ReplayListCtrl, customListCtrl)

  EVT_LIST_ITEM_SELECTED   ( RLIST_LIST, ReplayListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( RLIST_LIST, ReplayListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( RLIST_LIST, ReplayListCtrl::OnDeselected )
  EVT_LIST_ITEM_RIGHT_CLICK( RLIST_LIST, ReplayListCtrl::OnListRightClick )
  EVT_LIST_COL_CLICK       ( RLIST_LIST, ReplayListCtrl::OnColClick )
  EVT_MENU                 ( RLIST_DLMAP, ReplayListCtrl::OnDLMap )
  EVT_MENU                 ( RLIST_DLMOD, ReplayListCtrl::OnDLMod )
#if wxUSE_TIPWINDOW
#ifndef __WXMSW__ //disables tooltips on win
  EVT_MOTION(ReplayListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()

ReplayList* ReplayListCtrl::m_replaylist_sort = 0;

ReplayListCtrl::ReplayListCtrl( wxWindow* parent, ReplayList& replaylist  ):
  customListCtrl(parent, RLIST_LIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT),
  m_selected(-1),
  m_replaylist(replaylist)
{

  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );


  wxListItem col;

  col.SetText( _T("Date") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 0, col, _T("Date of recording") );

  col.SetText( _T("Mod") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 1, col, _T("Modname") );

  col.SetText( _T("Map") );
  col.SetImage(  icons().ICON_NONE);
  InsertColumn( 2, col, _T("Mapname") );

  col.SetText( _("Players") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 3, col, _T("Number of players") );

  col.SetText( _("Spring Version") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 4, col, _T("Spring Version") );

  col.SetText( _("File") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 5, col, _T("Filename") );


  m_sortorder[0].col = 0;
  m_sortorder[0].direction = true;
  m_sortorder[1].col = 1;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 2;
  m_sortorder[2].direction = true;
  m_sortorder[3].col = 3;
  m_sortorder[3].direction = true;
  Sort( );

#ifdef __WXMSW__
  SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );

#else
  SetColumnWidth( 0, 80 );
  SetColumnWidth( 1, 80 );
  SetColumnWidth( 2, 80 );

#endif

  SetColumnWidth( 3, 170 );
  SetColumnWidth( 4, 140 );

  m_popup = new wxMenu( _T("") );
  // &m enables shortcout "alt + m" and underlines m
  m_popup->Append( RLIST_DLMAP, _("Download &map") );
  m_popup->Append( RLIST_DLMOD, _("Download m&od") );
}


ReplayListCtrl::~ReplayListCtrl()
{
  delete m_popup;
}


void ReplayListCtrl::OnSelected( wxListEvent& event )
{
  m_selected = GetItemData( event.GetIndex() );
  event.Skip();
}


void ReplayListCtrl::OnDeselected( wxListEvent& event )
{
  if ( m_selected == (int)GetItemData( event.GetIndex() )  )
  m_selected = -1;
}


int ReplayListCtrl::GetSelectedIndex()
{
  return m_selected;
}


void ReplayListCtrl::OnListRightClick( wxListEvent& event )
{
  PopupMenu( m_popup );
}


void ReplayListCtrl::OnDLMap( wxCommandEvent& event )
{
//  if ( m_selected != -1 ) {
//    if ( m_ui.GetServer().replays_iter->ReplayExists(m_selected) ) {
//      m_ui.DownloadMap( m_ui.GetServer().replays_iter->GetReplay(m_selected).MapName );
//    }
//  }
}


void ReplayListCtrl::OnDLMod( wxCommandEvent& event )
{
//  if ( m_selected != -1 ) {
//    if ( m_ui.GetServer().replays_iter->ReplayExists(m_selected) ) {
//      m_ui.DownloadMod( m_ui.GetServer().replays_iter->GetReplay(m_selected).ModName );
//    }
//  }
}


void ReplayListCtrl::OnColClick( wxListEvent& event )
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


void ReplayListCtrl::Sort()
{
  ReplayListCtrl::m_replaylist_sort = &m_replaylist;
  if (m_replaylist_sort == 0 ) return;
  for (int i = 3; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&CompareDateUP:&CompareDateDOWN , 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareModUP:&CompareModDOWN , 0 ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&CompareMapUP:&CompareMapDOWN , 0 ); break;
      case 3 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayerUP:&ComparePlayerDOWN , 0 ); break;
      case 4 : SortItems( ( m_sortorder[ i ].direction )?&CompareSpringUP:&CompareSpringDOWN , 0 ); break;
      case 5 : SortItems( ( m_sortorder[ i ].direction )?&CompareFileUP:&CompareFileDOWN , 0 ); break;
    }
  }
}

int wxCALLBACK ReplayListCtrl::CompareMapUP(long item1, long item2, long sortData)
{

  Replay replay1 = m_replaylist_sort->GetReplay(item1);
  Replay replay2 = m_replaylist_sort->GetReplay(item2);

  if ( RefineMapname( replay1.MapName ).MakeUpper() < RefineMapname( replay2.MapName ).MakeUpper() )
      return -1;
  if ( RefineMapname( replay1.MapName ).MakeUpper() > RefineMapname( replay2.MapName ).MakeUpper() )
      return 1;

  return 0;
}

int wxCALLBACK ReplayListCtrl::CompareMapDOWN(long item1, long item2, long sortData)
{
  Replay replay1 = m_replaylist_sort->GetReplay(item1);
  Replay replay2 = m_replaylist_sort->GetReplay(item2);

  if ( RefineMapname( replay1.MapName ).MakeUpper() < RefineMapname( replay2.MapName ).MakeUpper() )
      return 1;
  if ( RefineMapname( replay1.MapName ).MakeUpper() > RefineMapname( replay2.MapName ).MakeUpper() )
      return -1;

  return 0;
}

int wxCALLBACK ReplayListCtrl::CompareModUP(long item1, long item2, long sortData)
{

  Replay replay1 = m_replaylist_sort->GetReplay(item1);
  Replay replay2 = m_replaylist_sort->GetReplay(item2);

  if ( RefineModname( replay1.ModName ).MakeUpper() < RefineModname( replay2.ModName ).MakeUpper() )
      return -1;
  if ( RefineModname( replay1.ModName ).MakeUpper() > RefineModname( replay2.ModName ).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK ReplayListCtrl::CompareModDOWN(long item1, long item2, long sortData)
{

  Replay replay1 = m_replaylist_sort->GetReplay(item1);
  Replay replay2 = m_replaylist_sort->GetReplay(item2);

  if ( RefineModname( replay1.ModName ).MakeUpper() < RefineModname( replay2.ModName ).MakeUpper() )
      return 1;
  if ( RefineModname( replay1.ModName ).MakeUpper() > RefineModname( replay2.ModName ).MakeUpper() )
      return -1;

  return 0;
}

int wxCALLBACK ReplayListCtrl::ComparePlayerUP(long item1, long item2, long sortData)
{

  Replay replay1 = m_replaylist_sort->GetReplay(item1);
  Replay replay2 = m_replaylist_sort->GetReplay(item2);

  if ( replay1.playernum < replay2.playernum )
      return -1;
  if ( replay1.playernum > replay2.playernum )
      return 1;

  return 0;
}

int wxCALLBACK ReplayListCtrl::ComparePlayerDOWN(long item1, long item2, long sortData)
{

  Replay replay1 = m_replaylist_sort->GetReplay(item1);
  Replay replay2 = m_replaylist_sort->GetReplay(item2);

  if ( replay1.playernum  < replay2.playernum  )
      return 1;
  if ( replay1.playernum > replay2.playernum  )
      return -1;

  return 0;
}

int wxCALLBACK ReplayListCtrl::CompareFileUP(long item1, long item2, long sortData)
{
    return 0;
}

int wxCALLBACK ReplayListCtrl::CompareFileDOWN(long item1, long item2, long sortData)
{
    return 0;
}

int wxCALLBACK ReplayListCtrl::CompareDateUP(long item1, long item2, long sortData)
{
    return 0;
}

int wxCALLBACK ReplayListCtrl::CompareDateDOWN(long item1, long item2, long sortData)
{
    return 0;
}

int wxCALLBACK ReplayListCtrl::CompareSpringUP(long item1, long item2, long sortData)
{
    return 0;
}

int wxCALLBACK ReplayListCtrl::CompareSpringDOWN(long item1, long item2, long sortData)
{
    return 0;
}

void ReplayListCtrl::OnMouseMotion(wxMouseEvent& event)
{
#if wxUSE_TIPWINDOW
	wxPoint position = event.GetPosition();

	try{
		tipTimer.Start(TOOLTIP_DELAY, wxTIMER_ONE_SHOT);
		int flag = wxLIST_HITTEST_ONITEM;
		long *ptrSubItem = new long;
#ifdef HAVE_WX28
		long item_hit = HitTest(position, flag, ptrSubItem);
#else
		long item_hit = HitTest(position, flag);
#endif

		if (item_hit != wxNOT_FOUND)
		{
			long item = GetItemData(item_hit);

			Replay replay = m_replaylist_sort->GetReplay(item);
			int coloumn = getColoumnFromPosition(position);
			switch (coloumn)
			{
			case 0: // date
			m_tiptext = replay.date;
				break;
			case 1: // modname
				m_tiptext = replay.ModName;
				break;
			case 2: // mapname
				m_tiptext = replay.MapName;
				break;
			case 3: //playernum
				m_tiptext = RefineModname(replay.ModName);
				break;
			case 4: // spring version
				m_tiptext = replay.SpringVersion;
				break;
			case 5: // filenam
				m_tiptext = replay.Filename;
				break;

			default: m_tiptext = _T("");
				break;
			}
		}
	}
	catch(...){}
#endif
}


