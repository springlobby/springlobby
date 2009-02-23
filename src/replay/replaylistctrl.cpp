/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/filename.h>
#include <wx/log.h>

#include "replaylistctrl.h"
#include "replaylist.h"
#include "../utils.h"
#include "../user.h"
#include "../iconimagelist.h"
#include "../uiutils.h"
#include "../ui.h"


BEGIN_EVENT_TABLE(ReplayListCtrl, CustomListCtrl)

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
  CustomListCtrl(parent, RLIST_LIST, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT,
                _T("replaylistctrl"), 8 ),
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

  col.SetText( _("Duration") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 4, col, _T("Duration") );

  col.SetText( _("Spring Version") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 5, col, _T("Spring Version") );

  col.SetText( _("Filesize") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 6, col, _T("Filesize in kilobyte") );

  col.SetText( _("File") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 7, col, _T("Filename") );


  m_sortorder[0].col = 0;
  m_sortorder[0].direction = false;
  m_sortorder[1].col = 1;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 2;
  m_sortorder[2].direction = true;
  m_sortorder[3].col = 3;
  m_sortorder[3].direction = true;
  Sort( );

#ifdef __WXMSW__
  SetColumnWidth( 3, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 5, wxLIST_AUTOSIZE_USEHEADER );

#else
  SetColumnWidth( 5, 50 );
  SetColumnWidth( 3, 50 );
#endif

  SetColumnWidth( 0, 80 );
  SetColumnWidth( 1, 140 );
  SetColumnWidth( 2, 140 );
  SetColumnWidth( 4, 160 );
  SetColumnWidth( 5, 80 );
  SetColumnWidth( 6, 70 );
  SetColumnWidth( 7, 180 );

  m_popup = new wxMenu( _T("") );
  // &m enables shortcout "alt + m" and underlines m
  m_popup->Append( RLIST_DLMAP, _("Download &map") );
  m_popup->Append( RLIST_DLMOD, _("Download m&od") );
}


ReplayListCtrl::~ReplayListCtrl()
{
  delete m_popup;
}

void ReplayListCtrl::OnListRightClick( wxListEvent& event )
{
  PopupMenu( m_popup );
}


void ReplayListCtrl::OnDLMap( wxCommandEvent& event )
{
    if ( m_selected != -1 ) {
        if ( m_replaylist.ReplayExists(m_selected) ) {
            OfflineBattle battle = m_replaylist.GetReplayById(m_selected).battle;
            ui().DownloadMap( battle.GetHostMapHash(), battle.GetHostMapName() );
        }
    }
}


void ReplayListCtrl::OnDLMod( wxCommandEvent& event )
{
    if ( m_selected != -1 ) {
        if ( m_replaylist.ReplayExists(m_selected) ) {
            OfflineBattle battle = m_replaylist.GetReplayById(m_selected).battle;
            ui().DownloadMod( battle.GetHostModHash(), battle.GetHostModName() );
        }
    }
}


void ReplayListCtrl::SetUnsorted(){
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( icons().ICON_NONE );
  SetColumn( m_sortorder[0].col, col );
}

void ReplayListCtrl::OnColClick( wxListEvent& event )
{
  int click_col=event.GetColumn();
  wxLogMessage(_T("click col: %d"),click_col);
  if ( click_col == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( icons().ICON_NONE );
  SetColumn( m_sortorder[0].col, col );

/*
  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}
  if ( i > 3 ) { i = 3; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;
*/
  if(click_col==m_sortorder[0].col){
    m_sortorder[0].direction=!m_sortorder[0].direction;
  }else{
    int order_remove=3;
    for(int i=0;i<4;++i){
      if(m_sortorder[i].col==click_col){
        order_remove=i;
      }
    }
    for(int i=order_remove;i>0;--i){
      m_sortorder[i]=m_sortorder[i-1];
    }
    m_sortorder[0].col=click_col;
    m_sortorder[0].direction=false;
  }

  for(int i=0;i<4;++i){
    wxLogMessage(_T("sorting level%d by %d direction %d"),i,m_sortorder[i].col,m_sortorder[i].direction);
  }

  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}

void ReplayListCtrl::Sort()//needs adjusting when column order etc is stable
{
//  ReplayListCtrl::m_replaylist_sort = &m_replaylist;
//  if (m_replaylist_sort == 0 ) return;
//  SortItems( CompareUniversal , (long)(m_sortorder) );
}

template<class T>
inline int MyCompare(T a, T b){
  return a<b?-1:(b<a?1:0);
}

/*
wxString duration = wxString::Format(_T("%02ld:%02ld:%02ld"), replay.duration / 3600,
                        (replay.duration%3600)/60, (replay.duration%60)/60 ) ;
    m_replay_listctrl->SetItem( index, 0, replay.date );
    m_replay_listctrl->SetItem( index, 1, replay.battle.GetHostModName() );
    m_replay_listctrl->SetItem( index, 2, replay.battle.GetHostMapName() );
    m_replay_listctrl->SetItem( index, 3, wxString::Format(_T("%d"),replay.battle.GetNumUsers() - replay.battle.GetSpectators() ) );
    m_replay_listctrl->SetItem( index, 4, duration );
    m_replay_listctrl->SetItem( index, 5, replay.SpringVersion );
    m_replay_listctrl->SetItem( index, 6, wxString::Format( _T("%d KB"),replay.size/1024 ) );
    m_replay_listctrl->SetItem( index, 7, replay.Filename.AfterLast( wxFileName::GetPathSeparator() ) );
*/


int wxCALLBACK ReplayListCtrl::CompareUniversal(long item1, long item2, long sortData){
  Replay replay1 ;
  Replay replay2 ;
  try{
    replay1 = m_replaylist_sort->GetReplayById(item1);
    replay2 = m_replaylist_sort->GetReplayById(item2);
  }catch(std::runtime_error){
    return 0;
  }
  SortOrder *m_sortorder=(SortOrder *)sortData;
  for(int i=0;i<4;++i){
    int c=0;
    switch ( m_sortorder[i].col ) {// switch is just a jump table, dont optimize anything here.
      case 0 : c=MyCompare(replay1.date,replay2.date) ; break;
      case 1 : c=MyCompare(replay1.battle.GetHostModName(),replay2.battle.GetHostModName()); break;
      case 2 : c=MyCompare(replay1.battle.GetHostMapName(),replay2.battle.GetHostMapName()); break;
      case 3 : c=MyCompare(replay1.battle.GetNumUsers() - replay1.battle.GetSpectators(), replay2.battle.GetNumUsers() - replay2.battle.GetSpectators()); break;
      case 4 : c=MyCompare(replay1.duration,replay2.duration); break;
      case 5 : c=MyCompare(replay1.SpringVersion, replay2.SpringVersion); break;
      case 6 : c=MyCompare(replay1.size, replay2.size); break;
      case 7 : c=MyCompare(replay1.Filename.AfterLast( wxFileName::GetPathSeparator() ), replay2.Filename.AfterLast( wxFileName::GetPathSeparator() )); break;
    }
    if(!m_sortorder[i].direction)c=-c;
    if(c!=0)return c;
  }
  return 0;
}

void ReplayListCtrl::OnMouseMotion(wxMouseEvent& event)
{
#if wxUSE_TIPWINDOW
	wxPoint position = event.GetPosition();

	try{
		m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
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

			Replay replay = m_replaylist.GetReplayById(item);
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


