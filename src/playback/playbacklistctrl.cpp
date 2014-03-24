/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/filename.h>
#include <wx/log.h>

#include "playbackstructs.h"
//#include "../utils.h"
#include "../user.h"
#include "../iconimagelist.h"
#include "../uiutils.h"
#include "../ui.h"
#include "utils/conversion.h"


BEGIN_EVENT_TABLE_TEMPLATE1(PlaybackListCtrl, PlaybackListCtrl::BaseType, PlaybackType )

  EVT_LIST_ITEM_RIGHT_CLICK( RLIST_LIST, PlaybackListCtrl::OnListRightClick )
  EVT_MENU                 ( RLIST_DLMAP, PlaybackListCtrl::OnDLMap )
  EVT_MENU                 ( RLIST_DLMOD, PlaybackListCtrl::OnDLMod )
  EVT_LIST_COL_CLICK       ( RLIST_LIST, ParentType::OnColClick )
  EVT_KEY_DOWN			   ( PlaybackListCtrl::OnChar )

END_EVENT_TABLE()

template<class T,class L> SortOrder CustomVirtListCtrl<T,L>::m_sortorder = SortOrder();

template <class PlaybackType>
PlaybackListCtrl<PlaybackType>::PlaybackListCtrl( wxWindow* parent  ):
  PlaybackListCtrl::BaseType(parent, RLIST_LIST, wxDefaultPosition, wxDefaultSize,
							wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT,
							_T("PlaybackListCtrl"), 4, &PlaybackListCtrl::CompareOneCrit )
{

	AddColumn( 0, wxLIST_AUTOSIZE, _("Date"), _("Date of recording") );
	AddColumn( 1, wxLIST_AUTOSIZE, _("Game"), _("Game name") );
	AddColumn( 2, wxLIST_AUTOSIZE, _("Map"), _("Mapname") );
	AddColumn( 3, wxLIST_AUTOSIZE_USEHEADER, _("Players"), _("Number of players") );
	AddColumn( 4, wxLIST_AUTOSIZE_USEHEADER, _("Duration"), _T("Duration") );
	AddColumn( 5, wxLIST_AUTOSIZE_USEHEADER, _("Version"), _("Version of the engine") );
	AddColumn( 6, wxLIST_AUTOSIZE, _("Filesize"), _("Filesize in kilobyte") );
	AddColumn( 7, wxLIST_AUTOSIZE, _("File"), _T("Filename") );

    if ( m_sortorder.size() == 0 ) {
      m_sortorder[0].col = 0;
      m_sortorder[0].direction = true;
      m_sortorder[1].col = 1;
      m_sortorder[1].direction = true;
      m_sortorder[2].col = 2;
      m_sortorder[2].direction = true;
      m_sortorder[3].col = 3;
      m_sortorder[3].direction = true;
      Sort( );
    }


    m_popup = new wxMenu( wxEmptyString );
    // &m enables shortcout "alt + m" and underlines m
    m_popup->Append( RLIST_DLMAP, _("Download &map") );
    m_popup->Append( RLIST_DLMOD, _("Download m&od") );
}

template <class PlaybackType>
PlaybackListCtrl<PlaybackType>::~PlaybackListCtrl()
{
  delete m_popup;
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::OnListRightClick( wxListEvent& /*unused*/ )
{
  PopupMenu( m_popup );
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::AddPlayback( const PlaybackType& replay )
{
	if ( AddItem( &replay ) ) {
		SetColumnWidth(0, wxLIST_AUTOSIZE);
		SetColumnWidth(1, wxLIST_AUTOSIZE);
		SetColumnWidth(2, wxLIST_AUTOSIZE);
		SetColumnWidth(6, wxLIST_AUTOSIZE);
		SetColumnWidth(7, wxLIST_AUTOSIZE);
		return;
	}

    wxLogWarning( _T("Replay already in list.") );
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::RemovePlayback( const PlaybackType& replay )
{
    if ( RemoveItem( &replay) )
        return;

    wxLogError( _T("Didn't find the replay to remove.") );
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::OnDLMap( wxCommandEvent& /*unused*/ )
{
    if ( m_selected_index > 0 &&  (long)m_data.size() > m_selected_index ) {
        OfflineBattle battle = m_data[m_selected_index]->battle;
        ui().Download( _T("map"), TowxString(battle.GetHostMapName()), TowxString(battle.GetHostMapHash()));
    }
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::OnDLMod( wxCommandEvent& /*unused*/ )
{
    if ( m_selected_index > 0 &&  (long)m_data.size() > m_selected_index ) {
        OfflineBattle battle = m_data[m_selected_index]->battle;
        ui().Download( _T("map"), TowxString(battle.GetHostModName()), TowxString(battle.GetHostModHash()) );
    }
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::Sort()
{
    if ( m_data.size() > 0 ) {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}

template <class PlaybackType>
int PlaybackListCtrl<PlaybackType>::CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const
{
    switch ( col ) {
        case 0: return dir * compareSimple( u1->date, u2->date );
        case 1: return dir * TowxString(u1->battle.GetHostModName()).CmpNoCase( TowxString(u2->battle.GetHostModName()) );
        case 2: return dir * TowxString(u1->battle.GetHostMapName()).CmpNoCase( TowxString(u2->battle.GetHostMapName()) );
        case 3: return dir * compareSimple( u1->battle.GetNumUsers() - u1->battle.GetSpectators(), u2->battle.GetNumUsers() - u2->battle.GetSpectators() );
        case 4: return dir * compareSimple( u1->duration,u2->duration );
        case 5: return dir * u1->SpringVersion.CmpNoCase( u2->SpringVersion ) ;
        case 6: return dir * compareSimple( u1->size, u2->size ) ;
        case 7: return dir * u1->Filename.AfterLast( wxFileName::GetPathSeparator() ).CmpNoCase( u2->Filename.AfterLast( wxFileName::GetPathSeparator() ) );
        default: return 0;
    }
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::SetTipWindowText( const long item_hit, const wxPoint& position)
{
    if ( item_hit < 0 || item_hit >= (long)m_data.size() )
        return;

    const PlaybackType& replay = *GetDataFromIndex( item_hit );

    int column = getColumnFromPosition( position );
    if (column > (int)m_colinfovec.size() || column < 0)
    {
        m_tiptext = wxEmptyString;
    }
    else
    {
        switch (column) {
            case 0: // date
            m_tiptext = replay.date_string;
                break;
            case 1: // modname
                m_tiptext = replay.ModName;
                break;
            case 2: // mapname
                m_tiptext = replay.MapName;
                break;
            case 3: //playernum
                m_tiptext = replay.ModName;
                break;
            case 4: // spring version
                m_tiptext = replay.SpringVersion;
                break;
            case 5: // filenam
                m_tiptext = replay.Filename;
                break;

            default: m_tiptext = wxEmptyString;
            break;
        }
    }
}

template <class PlaybackType>
wxString PlaybackListCtrl<PlaybackType>::GetItemText(long item, long column) const
{
    if ( m_data[item] == NULL )
        return wxEmptyString;

    const PlaybackType& replay = *m_data[item];
	wxString duration = wxFormat(_T("%02ld:%02ld:%02ld") )
									% (replay.duration / 3600)
									% ((replay.duration%3600)/60)
									% ((replay.duration%60)/60 ) ;

    switch ( column ) {
        case 0: return replay.date_string;
        case 1: return TowxString(replay.battle.GetHostModName());
        case 2: return TowxString(replay.battle.GetHostMapName());
		case 3: return wxFormat(_T("%d") ) % ( replay.battle.GetNumUsers() - replay.battle.GetSpectators() );
        case 4: return duration;
        case 5: return replay.SpringVersion;
		case 6: return wxFormat( _T("%d KB") ) % ( replay.size/1024 );
        case 7: return replay.Filename.AfterLast( wxFileName::GetPathSeparator() );

        default: return wxEmptyString;
    }
}

template <class PlaybackType>
int PlaybackListCtrl<PlaybackType>::GetItemImage(long item) const
{
    if ( m_data[item] == NULL )
        return -1;

    return -1;//icons().GetBattleStatusIcon( *m_data[item] );
}

template <class PlaybackType>
int PlaybackListCtrl<PlaybackType>::GetItemColumnImage(long /*item*/, long /*column*/) const
{
    //nothing's been done here atm
    return -1;

//    if ( m_data[item] == NULL )
//        return -1;
//
//    const PlaybackType& replay = *m_data[item];
//
//    switch ( column ) {
//        default: return -1;
//    }
}

template <class PlaybackType>
wxListItemAttr* PlaybackListCtrl<PlaybackType>::GetItemAttr(long /*unused*/) const
{
    //not neded atm
//    if ( item < m_data.size() && item > -1 ) {
//        const Replay& replay = *m_data[item];
//    }
    return NULL;
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::RemovePlayback( const int index )
{
    if ( index != -1 && index < long(m_data.size()) ) {
        m_data.erase( m_data.begin() + index );
        SetItemCount( m_data.size() );
        RefreshVisibleItems( );
        return;
    }
    wxLogError( _T("Didn't find the replay to remove.") );
}

template <class PlaybackType>
int PlaybackListCtrl<PlaybackType>::GetIndexFromData( const DataType& data ) const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it, ++i ) {
        if ( *it != 0 && data->Equals( *(*it) ) )
            return i;
    }
    wxLogError( _T("didn't find the battle.") );
    return -1;
}

template <class PlaybackType>
void PlaybackListCtrl<PlaybackType>::OnChar(wxKeyEvent & event)
{
	const int keyCode = event.GetKeyCode();
	if ( keyCode == WXK_DELETE )
		RemovePlayback( ParentType::GetSelectedIndex() );
	else
		event.Skip();
}

/////!TODO get rid of this in favor of the functionality in base class
//template <class PlaybackType>
//void PlaybackListCtrl::OnColClick( wxListEvent& event )
//{
//  int click_col=event.GetColumn();
//  wxLogMessage(_T("click col: %d"),click_col);
//  if ( click_col == -1 ) return;
//  wxListItem col;
//  GetColumn( m_sortorder[0].col, col );
//  col.SetImage( icons().ICON_NONE );
//  SetColumn( m_sortorder[0].col, col );
//
//  if(click_col==m_sortorder[0].col){
//    m_sortorder[0].direction *= -1;
//  }
//  else{
//    int order_remove=3;
//    for(int i=0;i<4;++i){
//      if(m_sortorder[i].col==click_col){
//        order_remove=i;
//      }
//    }
//    for(int i=order_remove;i>0;--i){
//      m_sortorder[i]=m_sortorder[i-1];
//    }
//    m_sortorder[0].col=click_col;
//    m_sortorder[0].direction=true;
//  }
//
//  for(int i=0;i<4;++i){
//    wxLogMessage(_T("sorting level%d by %d direction %d"),i,m_sortorder[i].col,m_sortorder[i].direction);
//  }
//
//  GetColumn( m_sortorder[0].col, col );
//  col.SetImage( ( m_sortorder[0].direction > 0 )?icons().ICON_UP:icons().ICON_DOWN );
//  SetColumn( m_sortorder[0].col, col );
//
//    SortList( true );
//}
