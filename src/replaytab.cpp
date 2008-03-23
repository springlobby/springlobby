#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <stdexcept>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif

#include "replaytab.h"
#include "replaylistctrl.h"

#include "ui.h"
#include "chatpanel.h"
#include "utils.h"
#include "uiutils.h"
#include "settings.h"
#include "iunitsync.h"
#include "mapctrl.h"

#include "replayfilter.h"
#include "iconimagelist.h"

#include "settings++/custom_dialogs.h"


#define REPLAYLIST_COLOUMNCOUNT 10

BEGIN_EVENT_TABLE(ReplayTab, wxPanel)

  EVT_BUTTON              ( REPLAY_WATCH             , ReplayTab::OnWatch        )
//  EVT_LIST_ITEM_ACTIVATED ( REPLAY_JOIN              , ReplayTab::OnListJoin    )
//  EVT_LIST_ITEM_SELECTED  ( BLIST_LIST               , ReplayTab::OnSelect      )
  EVT_CHECKBOX            ( REPLAY_LIST_FILTER_ACTIV , ReplayTab::OnFilterActiv )
#if  wxUSE_TOGGLEBTN
  EVT_TOGGLEBUTTON        ( REPLAY_LIST_FILTER_BUTTON, ReplayTab::OnFilter  )
#else
  EVT_CHECKBOX            ( REPLAY_LIST_FILTER_BUTTON , ReplayTab::OnFilter )
#endif


END_EVENT_TABLE()


ReplayTab::ReplayTab( wxWindow* parent, Ui& ui ) :
  wxPanel( parent, -1 ),
  m_ui(ui),
  m_sel_replay(0)
{
  m_replays = new ReplayList ();
  m_replays_iter = new ReplayList_Iter( m_replays );

  wxBoxSizer* m_main_sizer;
  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* m_filter_sizer;
  m_filter_sizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* m_replaylist_sizer;
  m_replaylist_sizer = new wxBoxSizer( wxVERTICAL );

  m_replay_listctrl = new ReplayListCtrl( this, *m_replays_iter );
  m_replaylist_sizer->Add( m_replay_listctrl, 1, wxALL|wxEXPAND, 5 );

  m_main_sizer->Add( m_replaylist_sizer, 1, wxEXPAND, 5 );;

  wxBoxSizer* m_info_sizer;
  m_info_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_minimap = new MapCtrl( this, 100, 0, m_ui, true, true, false, false );
  m_info_sizer->Add( m_minimap, 0, wxALL, 5 );

  wxFlexGridSizer* m_data_sizer;
  m_data_sizer = new wxFlexGridSizer( 4, 2, 0, 0 );

  m_map_lbl = new wxStaticText( this, wxID_ANY, _("Map:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_map_lbl, 0, wxALL, 5 );

  m_map_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_map_text, 0, wxALL, 5 );

  m_mod_lbl = new wxStaticText( this, wxID_ANY, _("Mod:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_mod_lbl, 0, wxALL, 5 );

  m_mod_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_mod_text, 0, wxALL, 5 );

  m_players_lbl = new wxStaticText( this, wxID_ANY, _("Players:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_players_lbl, 0, wxALL, 5 );

  m_players_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_players_text, 0, wxALL, 5 );

  m_info_sizer->Add( m_data_sizer, 1, wxEXPAND, 5 );

  m_main_sizer->Add( m_info_sizer, 0, wxEXPAND, 5 );


//  m_filter = new ReplayListFilter( this , wxID_ANY, this ,wxDefaultPosition, wxSize( -1,-1 ), wxEXPAND );
//  m_filter_sizer->Add( m_filter, 0, wxEXPAND, 5);
//
//  m_main_sizer->Add( m_filter_sizer, 0, wxEXPAND, 5);

  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_buttons_sizer;
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

#if  wxUSE_TOGGLEBTN
	m_filter_show = new wxToggleButton( this, REPLAY_LIST_FILTER_BUTTON , wxT(" Filter "), wxDefaultPosition , wxSize( -1,28 ), 0 );
#else
	m_filter_show = new wxCheckBox( this, REPLAY_LIST_FILTER_BUTTON , wxT(" Filter "), wxDefaultPosition , wxSize( -1,28 ), 0 );
#endif
  m_buttons_sizer->Add( m_filter_show, 0, 0, 5 );

	m_filter_activ = new wxCheckBox( this, REPLAY_LIST_FILTER_ACTIV , wxT("Activated"), wxDefaultPosition, wxDefaultSize, 0 );
  m_buttons_sizer->Add( m_filter_activ, 1, wxALL|wxEXPAND, 5 );
  #ifdef HAVE_WX26
  m_filter_activ->Disable();
  #endif

  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_watch_btn = new wxButton( this, REPLAY_WATCH, _("Watch"), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_watch_btn, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

  m_delete_btn = new wxButton( this, REPLAY_DELETE, _("Delete"), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_delete_btn, 0, wxBOTTOM|wxRIGHT, 5 );

  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

//  m_filter->Hide();

  this->SetSizer( m_main_sizer );
  this->Layout();

  SelectReplay(0);
}


ReplayTab::~ReplayTab()
{
    if (m_filter != 0)
        m_filter->SaveFilterValues();
}


void ReplayTab::SelectReplay( Replay* replay )
{
  m_sel_replay = replay;
//  m_minimap->SetReplay( m_sel_replay );
  //m_players->ClearUsers();
  if ( m_sel_replay != 0 ) {
    m_map_text->SetLabel( m_sel_replay->MapName );
    m_mod_text->SetLabel( m_sel_replay->ModName );
    m_players_text->SetLabel( wxString::Format( _T("%d"), m_sel_replay->playernum ) );

//    for ( unsigned int i = 0; i < m_sel_replay->playernum; i++ ) {
//      m_players->AddUser( m_sel_replay->playernames[ i ] );
//    }
  } else {
    m_map_text->SetLabel( wxEmptyString );
    m_mod_text->SetLabel( wxEmptyString );
    m_players_text->SetLabel(  _T("0") );
  }
}

void ReplayTab::AddReplay( Replay& replay ) {

//  if ( m_filter->GetActiv() && !m_filter->Filterreplay( replay ) ) {
//    return;
//  }
  int index = m_replay_listctrl->InsertItem( 0, replay.date );
  ASSERT_LOGIC( index != -1, _T("index = -1") );
  //TODO WUT
  m_replay_listctrl->SetItemData(index, (long)replay.id );

// doesnt have it?? !
  //replay.SetGUIListActiv( true );

  ASSERT_LOGIC( index != -1, _T("index = -1") );
  //wxListItem item;
  //item.SetId( index );

 // ASSERT_LOGIC( m_replay_listctrl->GetItem( item ), _T("!GetItem") );

  m_replay_listctrl->SetItem( index, 0, replay.date  );
  m_replay_listctrl->SetItem( index, 1, replay.ModName );
  m_replay_listctrl->SetItem( index, 2, replay.MapName );
  m_replay_listctrl->SetItem( index, 3, wxString::Format(_T("%d"),replay.playernum ) );
  m_replay_listctrl->SetItem( index, 4, replay.SpringVersion );
  m_replay_listctrl->SetItem( index, 5, replay.Filename );

  m_replay_listctrl->Sort();
}


void ReplayTab::RemoveReplay( Replay& replay ) {
  if ( &replay == m_sel_replay ) SelectReplay( 0 );
  for (int i = 0; i < m_replay_listctrl->GetItemCount() ; i++ ) {
    if ( replay.id == (int)m_replay_listctrl->GetItemData( i ) ) {
      m_replay_listctrl->DeleteItem( i );
      break;
    }
  }
  // TODOD
  //replay.SetGUIListActiv( false );
  m_replay_listctrl->Sort();
}

void ReplayTab::UpdateReplay( Replay& replay )
{
//  if ( !Replay.GetGUIListActiv() ) {
//    AddReplay( replay );
//    return;
//  }

//  if ( m_filter->GetActiv() && !m_filter->FilterReplay( replay ) ) {
//    RemoveReplay( replay );
//    return;
//  }

  int index = -1;
  for (int i = 0; i < m_replay_listctrl->GetItemCount() ; i++ ) {
    if ( replay.id == (int)m_replay_listctrl->GetItemData( i ) ) {
      index = i;
      break;
    }
  }

  ASSERT_LOGIC( index != -1, _T("index = -1") );

  //wxListItem item;
  //item.SetId( index );

  //ASSERT_LOGIC( m_replay_listctrl->GetItem( item ), _T("!GetItem") );

  //Replay& Replay = m_replays_iter.GetReplay( m_replay_listctrl->GetItemData( index ) );

  m_replay_listctrl->SetItem( index, 0, replay.date  );
  m_replay_listctrl->SetItem( index, 1, replay.ModName );
  m_replay_listctrl->SetItem( index, 2, replay.MapName );
  m_replay_listctrl->SetItem( index, 3, wxString::Format(_T("%d"),replay.playernum ) );
  m_replay_listctrl->SetItem( index, 4, replay.SpringVersion );
  m_replay_listctrl->SetItem( index, 5, replay.Filename );

  if ( &replay == m_sel_replay ) SelectReplay( m_sel_replay );
  m_replay_listctrl->Sort();

}


void ReplayTab::RemoveAllReplays() {
  SelectReplay( 0 );
  m_replays_iter->IteratorBegin();
  while (! m_replays_iter->EOL() ) {
    Replay temp_Replay = m_replays_iter->GetReplay();
//    if (temp_Replay != 0)
//        temp_Replay->SetGUIListActiv( false );
  }
  m_replay_listctrl->DeleteAllItems();
}


void ReplayTab::UpdateList() {
 m_replays_iter->IteratorBegin();
  while (!m_replays_iter->EOL() ) {
    Replay b = m_replays_iter->GetReplay();

    UpdateReplay(b);
  }
}


void ReplayTab::SetFilterActiv( bool activ )
{
  m_filter->SetActiv( activ );
  m_filter_activ->SetValue( activ );
}

void ReplayTab::OnFilter( wxCommandEvent& event )
{
  if (m_filter_show->GetValue()) {
    m_filter->Show(  );
    this->Layout();
  }
  else {
    m_filter->Hide(  );
    this->Layout();
  }
}

void ReplayTab::OnWatch( wxCommandEvent& event )
{

}

void ReplayTab::OnDelete( wxCommandEvent& event )
{

}

void ReplayTab::OnFilterActiv( wxCommandEvent& event )
{
  if ( !m_ui.IsConnected() )
  {
    m_filter_activ->SetValue( !m_filter_activ->GetValue() );
    return;
  }
  m_filter->SetActiv( m_filter_activ->GetValue() );
}

void ReplayTab::OnSelect( wxListEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( event.GetIndex() == -1 ) {
    SelectReplay( 0 );
  } else {
    SelectReplay( &m_replays_iter->GetReplay( m_replay_listctrl->GetItemData( event.GetIndex() ) ) );
  }
}


void ReplayTab::OnUnitSyncReloaded()
{
//  if ( ! m_ui.GetServerStatus() ) { return; }
//
//  m_replays_iter->IteratorBegin();
//  while (! m_replays_iter->EOL() ) {
//    Replay* b = m_replays_iter->GetReplay();
//    if (b!=0)
//        b->OnUnitSyncReloaded();
//  }
//  UpdateList();
//  m_minimap->UpdateMinimap();
}
