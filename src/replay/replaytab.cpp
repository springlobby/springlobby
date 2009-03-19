#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/filename.h>
#include <wx/log.h>
#if wxUSE_TOGGLEBTN
    #include <wx/tglbtn.h>
#endif

#include <stdexcept>

#include "replaytab.h"
#include "replaylistctrl.h"
#include "replaylist.h"
#include "../ui.h"
#include "../chatpanel.h"
#include "../utils.h"
#include "../uiutils.h"
#include "../settings.h"
#include "../iunitsync.h"
#include "../mapctrl.h"
#include "../battleroomlistctrl.h"

#include "replayfilter.h"
#include "../iconimagelist.h"

#include "../settings++/custom_dialogs.h"


const unsigned int REPLAYLIST_COLOUMNCOUNT = 10;

BEGIN_EVENT_TABLE(ReplayTab, wxPanel)

  EVT_BUTTON              ( REPLAY_WATCH             , ReplayTab::OnWatch        )
  EVT_BUTTON              ( REPLAY_RELOAD             , ReplayTab::OnReload        )
  EVT_BUTTON              ( REPLAY_DELETE            , ReplayTab::OnDelete    )
  EVT_LIST_ITEM_SELECTED  ( RLIST_LIST               , ReplayTab::OnSelect      )
// this doesn't get triggered (?)
  EVT_LIST_ITEM_DESELECTED( wxID_ANY               , ReplayTab::OnDeselect      )
  EVT_CHECKBOX            ( REPLAY_LIST_FILTER_ACTIV , ReplayTab::OnFilterActiv )
#if  wxUSE_TOGGLEBTN
  EVT_TOGGLEBUTTON        ( REPLAY_LIST_FILTER_BUTTON, ReplayTab::OnFilter  )
#else
  EVT_CHECKBOX            ( REPLAY_LIST_FILTER_BUTTON , ReplayTab::OnFilter )
#endif


END_EVENT_TABLE()

ReplayTab::ReplayTab( wxWindow* parent, Ui& ui ) :
  wxPanel( parent, -1 ),
  m_ui(ui)
{
    wxLogMessage(_T("ReplayTab::ReplayTab()"));

    m_replays = new ReplayList ( *this );

    wxBoxSizer* m_main_sizer;
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* m_filter_sizer;
    m_filter_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* m_replaylist_sizer;
    m_replaylist_sizer = new wxBoxSizer( wxVERTICAL );

    m_replay_listctrl = new ReplayListCtrl( this, *m_replays );
    m_replaylist_sizer->Add( m_replay_listctrl, 1, wxALL|wxEXPAND, 5 );

    m_main_sizer->Add( m_replaylist_sizer, 1, wxEXPAND, 5 );;

    wxBoxSizer* m_info_sizer;
    m_info_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_minimap = new MapCtrl( this, 100, 0, m_ui, true, true, false, false );
    m_info_sizer->Add( m_minimap, 0, wxALL, 5 );

    wxFlexGridSizer* m_data_sizer;
    m_data_sizer = new wxFlexGridSizer( 4, 2, 0, 0 );

    m_map_lbl = new wxStaticText( this, wxID_ANY, _("Map:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_data_sizer->Add( m_map_lbl, 1, wxALL|wxEXPAND, 5 );

    m_map_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_data_sizer->Add( m_map_text, 1, wxALL|wxEXPAND, 5 );

    m_mod_lbl = new wxStaticText( this, wxID_ANY, _("Mod:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_data_sizer->Add( m_mod_lbl, 1, wxALL|wxEXPAND, 5 );

    m_mod_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_data_sizer->Add( m_mod_text, 1, wxALL|wxEXPAND, 5 );

    m_players_lbl = new wxStaticText( this, wxID_ANY, _("Players:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_data_sizer->Add( m_players_lbl, 1, wxALL|wxEXPAND, 5 );

    m_players_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_data_sizer->Add( m_players_text, 1, wxALL|wxEXPAND, 5 );

    m_info_sizer->Add( m_data_sizer, 1, wxEXPAND|wxALL, 0 );

    m_players = new BattleroomListCtrl( this, 0, m_ui, true );
    m_info_sizer->Add( m_players , 2, wxALL|wxEXPAND, 0 );

    m_main_sizer->Add( m_info_sizer, 0, wxEXPAND, 5 );


    m_filter = new ReplayListFilter( this , wxID_ANY, this ,wxDefaultPosition, wxSize( -1,-1 ), wxEXPAND );
    m_filter_sizer->Add( m_filter, 0, wxEXPAND, 5);

    m_main_sizer->Add( m_filter_sizer, 0, wxEXPAND, 5);

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

    m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

    m_watch_btn = new wxButton( this, REPLAY_WATCH, _("Watch"), wxDefaultPosition, wxSize( -1,28 ), 0 );
    m_buttons_sizer->Add( m_watch_btn, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

    m_delete_btn = new wxButton( this, REPLAY_DELETE, _("Delete"), wxDefaultPosition, wxSize( -1,28 ), 0 );
    m_buttons_sizer->Add( m_delete_btn, 0, wxBOTTOM|wxRIGHT, 5 );

    m_reload_btn = new wxButton( this, REPLAY_RELOAD, _("Reload list"), wxDefaultPosition, wxSize( -1,28 ), 0 );
    m_buttons_sizer->Add( m_reload_btn, 0, wxBOTTOM|wxRIGHT, 5 );

    m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

    m_filter->Hide();

    this->SetSizer( m_main_sizer );
    this->Layout();

    //none selected --> shouldn't watch/delete that
    Deselect();

}


ReplayTab::~ReplayTab()
{
  m_minimap->SetBattle( NULL );
  if (m_filter != 0)
      m_filter->SaveFilterValues();
  if(m_replays)delete m_replays;
  wxLogMessage(_T("ReplayTab::!ReplayTab()"));
}

void ReplayTab::AddAllReplays()
{
    m_replays->LoadReplays();
}

void ReplayTab::AddReplay( Replay& replay ) {

    if ( m_filter->GetActiv() && !m_filter->FilterReplay( replay ) ) {
        return;
    }

    m_replay_listctrl->AddReplay( replay );
}


void ReplayTab::RemoveReplay( Replay& replay )
{
    int index = m_replay_listctrl->GetIndexFromData( &replay );

    if ( index == -1 )
        return;

    if ( index == m_replay_listctrl->GetSelectedIndex() )
        Deselect();

    m_replay_listctrl->RemoveReplay( replay );
}

void ReplayTab::UpdateReplay( Replay& replay )
{
    if ( m_filter->GetActiv() && !m_filter->FilterReplay( replay ) ) {
        RemoveReplay( replay );
        return;
    }

    int index = m_replay_listctrl->GetIndexFromData( &replay );

    if (index != -1)
        m_replay_listctrl->RefreshItem( index );
    else
        AddReplay( replay );

}

void ReplayTab::RemoveAllReplays()
{
    m_replay_listctrl->Clear();
    //shouldn't list be cleared too here? (koshi)
}


void ReplayTab::UpdateList()
{
    replay_map_t &replays=m_replays->GetReplaysMap();
    for(replay_iter_t i=replays.begin();i!=replays.end();++i){
        UpdateReplay(i->second);
    }
    m_replay_listctrl->RefreshVisibleItems();
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
    if (m_replay_listctrl->GetSelectedIndex() != -1 ) {
         int m_sel_replay_id = m_replay_listctrl->GetSelectedData()->id;


        wxLogMessage(_T("Watching replay %d "),m_sel_replay_id);
        try{
            Replay& rep = m_replays->GetReplayById( m_sel_replay_id );

            std::map<wxString, wxString> versionlist = sett().GetSpringVersionList();
            if ( versionlist.size() == 0 )
            {
              wxLogWarning( _T("can't get spring version from any unitsync") );
              customMessageBoxNoModal(SL_MAIN_ICON,  _("Couldn't get your spring versions from any unitsync library."), _("Spring error"), wxICON_EXCLAMATION|wxOK );
              return;
            }
            bool versionfound = false;
            for ( std::map<wxString, wxString>::iterator itor = versionlist.begin(); itor != versionlist.end(); itor++ )
            {
              if ( itor->second == rep.SpringVersion )
              {
                if ( sett().GetCurrentUsedSpringIndex() != itor->first )
                {
                  wxLogMessage(_T("replay requires version: %s, switching to profile: %s"), rep.SpringVersion.c_str(), itor->first.c_str() );
                  sett().SetUsedSpringIndex( itor->first );
                  ui().ReloadUnitSync();
                }
                versionfound = true;
              }
            }
            if ( !versionfound )
            {
              wxString message = wxString::Format( _("No compatible installed spring version has been found, this replay requires version: %s\n"), rep.SpringVersion.c_str() );
              message << _("Your current installed versions are:");
              for ( std::map<wxString, wxString>::iterator itor = versionlist.begin(); itor != versionlist.end(); itor++ ) message << _T(" ") << itor->second;
              customMessageBoxNoModal (SL_MAIN_ICON, message, _("Spring error"), wxICON_EXCLAMATION|wxOK );
              wxLogWarning ( _T("no spring version supported by this replay found") );
              return;
            }

            bool watchable = rep.battle.MapExists() && rep.battle.ModExists();
            if ( watchable )
                m_ui.WatchReplay( rep.Filename );
            else {
                #ifdef NO_TORRENT_SYSTEM
                    wxString downloadProc = _("Do you want me to take you to the download page?");
                #else
                    wxString downloadProc = _("Should i try to download it for you?\nYou can see the progress in the \"Download Manager\" tab.");
                #endif

                OfflineBattle& battle = rep.battle;

                if ( !battle.ModExists() ) {
                    if (customMessageBox( SL_MAIN_ICON, _("You need to download the mod before you can watch this replay.\n\n") + downloadProc, _("Mod not available"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
                        wxString modhash = battle.GetHostModHash();
                        wxString modname = battle.GetHostModName();
                        m_ui.DownloadMod ( modhash, modname );
                    }
                    return;
                }

                if ( !battle.MapExists() ) {
                    if (customMessageBox(SL_MAIN_ICON, _("You need to download the map to be able to watch this replay.\n\n") + downloadProc, _("Map not available"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
                        wxString maphash = battle.GetHostMapHash();
                        wxString mapname = battle.GetHostMapName();
                        m_ui.DownloadMap ( maphash, mapname );
                    }
                }
            }
        }catch(std::runtime_error){
            return;
        }
    }else{
        Deselected();
    }
}

void ReplayTab::OnDelete( wxCommandEvent& event )
{
    int sel_index=m_replay_listctrl->GetSelectedIndex();
    if ( sel_index >=0 ) {
        try{
            int m_sel_replay_id = m_replay_listctrl->GetSelectedData()->id;
            wxLogMessage(_T("Deleting replay %d "),m_sel_replay_id);
            Replay& rep = m_replays->GetReplayById( m_sel_replay_id );
            wxString fn = rep.Filename;
            if ( !m_replays->DeleteReplay( m_sel_replay_id ) )
                customMessageBoxNoModal(SL_MAIN_ICON, _("Could not delete Replay: ") + fn,
                    _("Error") );
            else{
                RemoveReplay( rep );
                Deselect();
            }
        }catch(std::runtime_error){
            return;
        }
    }else{
        Deselected();
    }
}

void ReplayTab::OnFilterActiv( wxCommandEvent& event )
{
    m_filter->SetActiv( m_filter_activ->GetValue() );
}

void ReplayTab::OnSelect( wxListEvent& event )
{
    wxLogDebugFunc( _T("") );
    if ( event.GetIndex() == -1 ) {
        Deselect();
    }
    else
    {
        try
        {
            m_watch_btn->Enable( true );
            m_delete_btn->Enable( true );
            int index = event.GetIndex();
            m_replay_listctrl->SetSelectedIndex( index );

            //this might seem a bit backwards, but it's currently the only way that doesn't involve casting away constness
            int m_sel_replay_id = m_replay_listctrl->GetDataFromIndex( index )->id;
            Replay& rep = m_replays->GetReplayById( m_sel_replay_id );


            wxLogMessage(_T("Selected replay %d "),m_sel_replay_id);

            m_players_text->SetLabel(_T(""));
            m_map_text->SetLabel(rep.battle.GetHostMapName());
            m_mod_text->SetLabel(rep.battle.GetHostModName());
            m_minimap->SetBattle( &(rep.battle) );
            m_minimap->UpdateMinimap();
            try
            {
							IBattle& oldbattle = m_players->GetBattle();
							for ( size_t i = 0; i < oldbattle.GetNumUsers(); ++i )
							{
									try
									{
										User& usr = oldbattle.GetUser( i );
										m_players->RemoveUser( usr );
									}
									catch(...)
									{}
							}
            } catch ( assert_exception ) {}
            m_players->DeleteAllItems();
            m_players->SetBattle( (IBattle*)&rep.battle );
            for ( size_t i = 0; i < rep.battle.GetNumUsers(); ++i )
            {
								try
								{
									User& usr = rep.battle.GetUser( i );
									m_players->AddUser( usr );
								}
								catch(...)
								{}
            }
        }
        catch ( ... ) {
          Deselect();
        }
        event.Skip();
    }
}

void ReplayTab::OnDeselect( wxListEvent& event )
{
    wxLogMessage(_T("ReplayTab::OnDeselect"));
    Deselected();
}

void ReplayTab::Deselect()
{
    m_replay_listctrl->SelectNone();
    Deselected();
}

void ReplayTab::Deselected()
{
    m_watch_btn->Enable( false );
    m_delete_btn->Enable( false );
    m_minimap->SetBattle( NULL );
    m_players->DeleteAllItems();
    m_players->SetBattle( NULL );
}

void ReplayTab::ReloadList()
{
    wxDateTime dt = wxDateTime::UNow();
    Deselect();
    m_replays->RemoveAll();
    m_replays->LoadReplays();

    long sec = (wxDateTime::UNow() - dt).GetMilliseconds().ToLong();
    if ( sec > 0 )
        customMessageBoxNoModal(SL_MAIN_ICON, wxString::Format( _T("List reloaded in %d milli seconds"),sec ) );
}

void ReplayTab::OnReload( wxCommandEvent& event )
{
    ReloadList();
}
